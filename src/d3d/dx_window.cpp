#include "dx_window.h"

#include "auxiliary/baltic_except.h"
#include "d3d/dx_context.h"

#define IDI_APPLICATION_W MAKEINTRESOURCEW(32512)
#define IDC_ARROW_W MAKEINTRESOURCEW(32512)

namespace Baltic
{
    LRESULT OnWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if (msg == WM_SIZE) {
            DXWindow* windowPtr;
            if (!(windowPtr = reinterpret_cast<DXWindow*>(GetWindowLongPtrW(wnd, GWLP_USERDATA)))) {
                throw BalticException("GetWindowLongPtrW");
            }

            if (LOWORD(lParam) && HIWORD(lParam) &&
                (LOWORD(lParam) != windowPtr->m_width || HIWORD(lParam) != windowPtr->m_height)) {
                windowPtr->m_shouldResize = TRUE;
            }

            return DefWindowProcW(wnd, msg, wParam, lParam);
        }
        else if (msg == WM_KEYDOWN) {
            DXWindow* windowPtr;
            if (!(windowPtr = reinterpret_cast<DXWindow*>(GetWindowLongPtrW(wnd, GWLP_USERDATA)))) {
                throw BalticException("GetWindowLongPtrW");
            }

            if (wParam == VK_F11) {
                windowPtr->SetFullscreen(!windowPtr->isFullscreen());
            }

            return DefWindowProcW(wnd, msg, wParam, lParam);
        }
        else if (msg == WM_CLOSE) {
            DXWindow* windowPtr;
            if (!(windowPtr = reinterpret_cast<DXWindow*>(GetWindowLongPtrW(wnd, GWLP_USERDATA)))) {
                throw BalticException("GetWindowLongPtrW");
            }
            windowPtr->m_shouldClose = TRUE;

            return 0;
        }
        else if (msg == WM_NCCREATE) {
            auto* createPtr = reinterpret_cast<CREATESTRUCT*>(lParam);
            auto* windowPtr = reinterpret_cast<DXWindow*>(createPtr->lpCreateParams);
            SetWindowLongPtrW(wnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(windowPtr));

            return DefWindowProcW(wnd, msg, wParam, lParam);
        }
        else {
            return DefWindowProcW(wnd, msg, wParam, lParam);
        }
    }

    DXWindow::DXWindow(UINT width, UINT height, DXContext& dxContext)
        : m_wndClass(0),
          m_window(nullptr),
          m_width(width),
          m_height(height),
          m_shouldClose(FALSE),
          m_shouldResize(FALSE),
          m_isFullscreen(FALSE),
          m_currentBufferIdx(0)
    {
        WNDCLASSEXW wcex = {
            .cbSize = sizeof(wcex),
            .style = CS_OWNDC,
            .lpfnWndProc = &OnWindowMessage,
            .cbClsExtra = 0,
            .cbWndExtra = sizeof(LONG_PTR),
            .hInstance = GetModuleHandleW(nullptr),
            .hIcon = LoadIconW(nullptr, IDI_APPLICATION_W),
            .hCursor = LoadCursorW(nullptr, IDC_ARROW_W),
            .hbrBackground = nullptr,
            .lpszMenuName = nullptr,
            .lpszClassName = L"BalticWndCls",
            .hIconSm = LoadIconW(nullptr, IDI_APPLICATION_W)
        };

        if (!(m_wndClass = RegisterClassExW(&wcex))) {
            throw BalticException("RegisterClassExW");
        }

        if (!(m_window = CreateWindowExW(
                  WS_EX_OVERLAPPEDWINDOW | WS_EX_APPWINDOW, reinterpret_cast<LPCWSTR>(m_wndClass), L"Baltic",
                  WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 1920, 1080, nullptr, nullptr, wcex.hInstance, this
              ))) {
            throw BalticException("CreateWindowExW");
        }

        const auto& factory = dxContext.GetFactoryComPtr();

        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {
            .Width = 1920,
            .Height = 1080,
            .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
            .Stereo = false,
            .SampleDesc = {.Count = 1, .Quality = 0},
            .BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT,
            .BufferCount = FRAME_COUNT,
            .Scaling = DXGI_SCALING_STRETCH,
            .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
            .AlphaMode = DXGI_ALPHA_MODE_IGNORE,
            .Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING
        };

        DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFullscreenDesc = {.Windowed = true};

        ComPtr<IDXGISwapChain1> swapChain1;

        if (FAILED(factory->CreateSwapChainForHwnd(
                dxContext.GetCmdQueueComPtr().Get(), m_window, &swapChainDesc, &swapChainFullscreenDesc, nullptr,
                &swapChain1
            ))) {
            throw BalticException("factory->CreateSwapChainForHwnd");
        }

        if (FAILED(swapChain1.As(&m_swapChain))) {
            throw BalticException("swapChain1.As");
        }

        D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {
            .Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
            .NumDescriptors = FRAME_COUNT,
            .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
            .NodeMask = 0
        };

        if (FAILED(dxContext.GetDeviceComPtr()->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_rtvDescHeap))
            )) {
            throw BalticException("dxContext.GetDevice()->CreateDescriptorHeap");
        }

        D3D12_CPU_DESCRIPTOR_HANDLE firstHandle = m_rtvDescHeap->GetCPUDescriptorHandleForHeapStart();
        UINT handleInc = dxContext.GetDeviceComPtr()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        for (UINT i = 0; i < FRAME_COUNT; i++) {
            m_rtvHandles[i] = firstHandle;
            m_rtvHandles[i].ptr += handleInc * i;
        }

        GetBuffers(dxContext.GetDeviceComPtr().Get());
    }

    DXWindow::~DXWindow()
    {
        if (m_window) {
            DestroyWindow(m_window);
        }

        if (m_wndClass) {
            UnregisterClassW(reinterpret_cast<LPCWSTR>(m_wndClass), GetModuleHandleW(nullptr));
        }
    }

    void DXWindow::Update()
    {
        MSG msg;
        while (PeekMessageW(&msg, m_window, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);

            if (FAILED(DispatchMessageW(&msg))) {
                throw BalticException("DispatchMessageW");
            }
        }
    }

    void DXWindow::Present()
    {
        if (FAILED(m_swapChain->Present(1, 0))) {
            throw BalticException("m_swapChain->Present");
        }
    }

    void DXWindow::ResizeSwapChain(ID3D12Device8* device)
    {
        RECT clientRect;
        if (!GetClientRect(m_window, &clientRect)) {
            throw BalticException("GetClientRect");
        }

        m_width = clientRect.right - clientRect.left;
        m_height = clientRect.bottom - clientRect.top;

        ReleaseBuffers();

        if (FAILED(m_swapChain->ResizeBuffers(
                FRAME_COUNT, m_width, m_height, DXGI_FORMAT_UNKNOWN,
                DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING
            ))) {
            throw BalticException("m_swapChain->ResizeBuffers");
        }

        GetBuffers(device);

        m_shouldResize = FALSE;
    }

    void DXWindow::SetFullscreen(BOOL enable)
    {
        DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
        DWORD exStyle = WS_EX_OVERLAPPEDWINDOW | WS_EX_APPWINDOW;

        if (enable) {
            style = WS_POPUP | WS_VISIBLE;
            exStyle = WS_EX_APPWINDOW;
        }

        SetWindowLongPtrW(m_window, GWL_STYLE, style);
        SetWindowLongPtrW(m_window, GWL_EXSTYLE, exStyle);

        m_isFullscreen = enable;

        if (enable) {
            HMONITOR monitor = MonitorFromWindow(m_window, MONITOR_DEFAULTTONEAREST);
            MONITORINFO monitorInfo = {.cbSize = sizeof(monitorInfo)};
            if (!GetMonitorInfoW(monitor, &monitorInfo)) {
                throw BalticException("GetMonitorInfoW");
            }

            if (!SetWindowPos(
                    m_window, nullptr, monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top,
                    monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
                    monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top, SWP_NOZORDER
                )) {
                throw BalticException("SetWindowPos");
            }
        }
        else {
            ShowWindow(m_window, SW_MAXIMIZE);
        }
    }

    void DXWindow::BeginFrame(ID3D12GraphicsCommandList6* cmdList)
    {
        m_currentBufferIdx = m_swapChain->GetCurrentBackBufferIndex();

        D3D12_RESOURCE_BARRIER resourceBarrier = {
            .Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
            .Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
            .Transition =
                D3D12_RESOURCE_TRANSITION_BARRIER{
                    .pResource = m_buffers[m_currentBufferIdx].Get(),
                    .Subresource = 0,
                    .StateBefore = D3D12_RESOURCE_STATE_PRESENT,
                    .StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET
                }
        };

        cmdList->ResourceBarrier(1, &resourceBarrier);

        FLOAT clearColor[] = {.1f, .1f, .1f, 1.f};
        cmdList->ClearRenderTargetView(m_rtvHandles[m_currentBufferIdx], clearColor, 0, nullptr);

        cmdList->OMSetRenderTargets(1, &m_rtvHandles[m_currentBufferIdx], FALSE, nullptr);
    }

    void DXWindow::EndFrame(ID3D12GraphicsCommandList6* cmdList)
    {
        D3D12_RESOURCE_BARRIER resourceBarrier = {
            .Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
            .Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
            .Transition =
                D3D12_RESOURCE_TRANSITION_BARRIER{
                    .pResource = m_buffers[m_currentBufferIdx].Get(),
                    .Subresource = 0,
                    .StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET,
                    .StateAfter = D3D12_RESOURCE_STATE_PRESENT
                }
        };

        cmdList->ResourceBarrier(1, &resourceBarrier);
    }

    void DXWindow::GetBuffers(ID3D12Device8* device)
    {
        for (UINT i = 0; i < FRAME_COUNT; i++) {
            if (FAILED(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_buffers[i])))) {
                throw BalticException("m_swapChain->GetBuffer");
            }

            D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {
                .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
                .ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D,
                .Texture2D = {.MipSlice = 0, .PlaneSlice = 0}
            };

            device->CreateRenderTargetView(m_buffers[i].Get(), &rtvDesc, m_rtvHandles[i]);
        }
    }

    void DXWindow::ReleaseBuffers()
    {
        for (auto& m_buffer : m_buffers) {
            m_buffer.Reset();
        }
    }

} // namespace Baltic
