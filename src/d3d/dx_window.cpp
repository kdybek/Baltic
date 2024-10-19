#include "dx_window.hpp"

#include "auxiliary/baltic_exception.hpp"
#include "d3d/dx_context.hpp"
#include "d3d/dx_resource.hpp"
#include "imgui.h"

LRESULT EventQueueWindowProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    try {
        if (msg == WM_MOUSEMOVE) {
            DXWindow* windowPtr;
            if (!(windowPtr = reinterpret_cast<DXWindow*>(GetWindowLongPtr(wnd, GWLP_USERDATA)))) {
                throw GenericException(TEXT("GetWindowLongPtr"));
            }

            windowPtr->m_eventQueue.push(
                {.type = EventType::MouseMove, .cursorPosition = {.x = LOWORD(lParam), .y = HIWORD(lParam)}}
            );

            return DefWindowProc(wnd, msg, wParam, lParam);
        }
        else if (msg == WM_KEYDOWN) {
            DXWindow* windowPtr;
            if (!(windowPtr = reinterpret_cast<DXWindow*>(GetWindowLongPtr(wnd, GWLP_USERDATA)))) {
                throw GenericException(TEXT("GetWindowLongPtr"));
            }

            switch (wParam) {
                case 'W':
                    windowPtr->m_eventQueue.push({.type = EventType::KeyDown, .key = Key::W});
                    break;
                case 'A':
                    windowPtr->m_eventQueue.push({.type = EventType::KeyDown, .key = Key::A});
                    break;
                case 'S':
                    windowPtr->m_eventQueue.push({.type = EventType::KeyDown, .key = Key::S});
                    break;
                case 'D':
                    windowPtr->m_eventQueue.push({.type = EventType::KeyDown, .key = Key::D});
                    break;
                case VK_SPACE:
                    windowPtr->m_eventQueue.push({.type = EventType::KeyDown, .key = Key::Space});
                    break;
                case VK_SHIFT:
                    windowPtr->m_eventQueue.push({.type = EventType::KeyDown, .key = Key::Shift});
                    break;
                case VK_F11:
                    windowPtr->m_eventQueue.push({.type = EventType::KeyDown, .key = Key::F11});
                    break;
                case VK_ESCAPE:
                    windowPtr->m_eventQueue.push({.type = EventType::KeyDown, .key = Key::Escape});
                    break;
            }

            return DefWindowProc(wnd, msg, wParam, lParam);
        }
        else if (msg == WM_KEYUP) {
            DXWindow* windowPtr;
            if (!(windowPtr = reinterpret_cast<DXWindow*>(GetWindowLongPtr(wnd, GWLP_USERDATA)))) {
                throw GenericException(TEXT("GetWindowLongPtr"));
            }

            switch (wParam) {
                case 'W':
                    windowPtr->m_eventQueue.push({.type = EventType::KeyUp, .key = Key::W});
                    break;
                case 'A':
                    windowPtr->m_eventQueue.push({.type = EventType::KeyUp, .key = Key::A});
                    break;
                case 'S':
                    windowPtr->m_eventQueue.push({.type = EventType::KeyUp, .key = Key::S});
                    break;
                case 'D':
                    windowPtr->m_eventQueue.push({.type = EventType::KeyUp, .key = Key::D});
                    break;
                case VK_SPACE:
                    windowPtr->m_eventQueue.push({.type = EventType::KeyUp, .key = Key::Space});
                    break;
                case VK_SHIFT:
                    windowPtr->m_eventQueue.push({.type = EventType::KeyUp, .key = Key::Shift});
                    break;
                case VK_F11:
                    windowPtr->m_eventQueue.push({.type = EventType::KeyUp, .key = Key::F11});
                    break;
                case VK_ESCAPE:
                    windowPtr->m_eventQueue.push({.type = EventType::KeyUp, .key = Key::Escape});
                    break;
            }

            return DefWindowProc(wnd, msg, wParam, lParam);
        }
        if (msg == WM_SIZE) {
            DXWindow* windowPtr;
            if (!(windowPtr = reinterpret_cast<DXWindow*>(GetWindowLongPtr(wnd, GWLP_USERDATA)))) {
                throw GenericException(TEXT("GetWindowLongPtr"));
            }

            if (LOWORD(lParam) && HIWORD(lParam) &&
                (LOWORD(lParam) != windowPtr->m_width || HIWORD(lParam) != windowPtr->m_height)) {
                windowPtr->m_eventQueue.push({.type = EventType::Resize});
            }

            return DefWindowProc(wnd, msg, wParam, lParam);
        }
        else if (msg == WM_MOVE) {
            DXWindow* windowPtr;
            if (!(windowPtr = reinterpret_cast<DXWindow*>(GetWindowLongPtr(wnd, GWLP_USERDATA)))) {
                throw GenericException(TEXT("GetWindowLongPtr"));
            }

            windowPtr->m_eventQueue.push({.type = EventType::Move});

            return DefWindowProc(wnd, msg, wParam, lParam);
        }
        else if (msg == WM_SETFOCUS) {
            DXWindow* windowPtr;
            if (!(windowPtr = reinterpret_cast<DXWindow*>(GetWindowLongPtr(wnd, GWLP_USERDATA)))) {
                throw GenericException(TEXT("GetWindowLongPtr"));
            }

            windowPtr->m_eventQueue.push({.type = EventType::Focus});

            return DefWindowProc(wnd, msg, wParam, lParam);
        }
        else if (msg == WM_KILLFOCUS) {
            DXWindow* windowPtr;
            if (!(windowPtr = reinterpret_cast<DXWindow*>(GetWindowLongPtr(wnd, GWLP_USERDATA)))) {
                throw GenericException(TEXT("GetWindowLongPtr"));
            }

            windowPtr->m_eventQueue.push({.type = EventType::Blur});

            return DefWindowProc(wnd, msg, wParam, lParam);
        }
        else if (msg == WM_CLOSE) {
            DXWindow* windowPtr;
            if (!(windowPtr = reinterpret_cast<DXWindow*>(GetWindowLongPtr(wnd, GWLP_USERDATA)))) {
                throw GenericException(TEXT("GetWindowLongPtr"));
            }

            windowPtr->m_eventQueue.push({.type = EventType::Close});

            return TRUE;
        }
        else if (msg == WM_NCCREATE) {
            auto* createPtr = reinterpret_cast<CREATESTRUCT*>(lParam);
            auto* windowPtr = reinterpret_cast<DXWindow*>(createPtr->lpCreateParams);
            SetWindowLongPtr(wnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(windowPtr));

            return DefWindowProc(wnd, msg, wParam, lParam);
        }
        else {
            return DefWindowProc(wnd, msg, wParam, lParam);
        }
    }
    catch (const BalticException& e) {
#ifdef UNICODE
        OutputDebugStringW((L"WinProc error: " + std::wstring(e.GetMessage())).c_str());
#else
        OutputDebugStringA(("WinProc error: " + std::string(e.GetMessage())).c_str());
#endif
        return FALSE;
    }
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK ImGuiWindowProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(wnd, msg, wParam, lParam)) {
        return TRUE;
    }

    return EventQueueWindowProc(wnd, msg, wParam, lParam);
}

DXWindow::DXWindow(
    HINSTANCE instance, ATOM wndClass, const TCHAR* wndName, UINT width, UINT height, DXContext& dxContext
)
    : m_wndClass(wndClass),
      m_windowHandle(nullptr),
      m_width(width),
      m_height(height),
      m_viewport{
          .TopLeftX = 0.f,
          .TopLeftY = 0.f,
          .Width = static_cast<FLOAT>(width),
          .Height = static_cast<FLOAT>(height),
          .MinDepth = 0.f,
          .MaxDepth = 1.f
      },
      m_scissorRect{.left = 0, .top = 0, .right = static_cast<LONG>(width), .bottom = static_cast<LONG>(height)},
      m_isFullscreen(FALSE),
      m_cursorVisible(TRUE)
{
    if (!(m_windowHandle = CreateWindowEx(
              WS_EX_OVERLAPPEDWINDOW | WS_EX_APPWINDOW, MAKEINTATOM(m_wndClass), wndName,
              WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, nullptr,
              nullptr, instance, this
          ))) {
        throw GenericException(TEXT("CreateWindowEx"));
    }

    const auto& factory = dxContext.GetFactoryComPtr();

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc{
        .Width = width,
        .Height = height,
        .Format = RTV_FORMAT,
        .Stereo = false,
        .SampleDesc{.Count = 1, .Quality = 0},
        .BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT,
        .BufferCount = FRAME_COUNT,
        .Scaling = DXGI_SCALING_STRETCH,
        .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
        .AlphaMode = DXGI_ALPHA_MODE_IGNORE,
        .Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING
    };

    DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFullscreenDesc{.Windowed = true};

    ComPtr<IDXGISwapChain1> swapChain1;

    DXThrowIfFailed(factory->CreateSwapChainForHwnd(
        dxContext.GetCmdQueueComPtr().Get(), m_windowHandle, &swapChainDesc, &swapChainFullscreenDesc, nullptr,
        &swapChain1
    ));

    DXThrowIfFailed(swapChain1.As(&m_swapChain));

    D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc{
        .Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
        .NumDescriptors = FRAME_COUNT,
        .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
        .NodeMask = 0
    };

    D3D12_DESCRIPTOR_HEAP_DESC dsvDescriptorHeapDesc{
        .Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
        .NumDescriptors = 1,
        .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
        .NodeMask = 0
    };

    DXThrowIfFailed(
        dxContext.GetDeviceComPtr()->CreateDescriptorHeap(&rtvDescriptorHeapDesc, IID_PPV_ARGS(&m_rtvDescHeap))
    );

    D3D12_CPU_DESCRIPTOR_HANDLE firstHandle = m_rtvDescHeap->GetCPUDescriptorHandleForHeapStart();
    UINT handleInc = dxContext.GetDeviceComPtr()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    for (UINT i = 0; i < FRAME_COUNT; i++) {
        m_rtvHandles[i] = firstHandle;
        m_rtvHandles[i].ptr += handleInc * i;
    }

    DXThrowIfFailed(
        dxContext.GetDeviceComPtr()->CreateDescriptorHeap(&dsvDescriptorHeapDesc, IID_PPV_ARGS(&m_dsvHeapDesc))
    );

    m_dsvHandle = m_dsvHeapDesc->GetCPUDescriptorHandleForHeapStart();

    GetBuffers(dxContext.GetDeviceComPtr().Get());
}

DXWindow::~DXWindow()
{
    if (m_windowHandle) {
        DestroyWindow(m_windowHandle);
    }
}

void DXWindow::Update()
{
    MSG msg;
    while (PeekMessage(&msg, m_windowHandle, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DXThrowIfFailed(DispatchMessage(&msg));
    }
}

void DXWindow::Present() { DXThrowIfFailed(m_swapChain->Present(1, 0)); }

Event DXWindow::PollEvent()
{
    if (m_eventQueue.empty()) {
        return {.type = EventType::None};
    }

    Event event = m_eventQueue.front();
    m_eventQueue.pop();

    return event;
}

void DXWindow::Resize(ID3D12Device10* device)
{
    RECT clientRect;
    if (!GetClientRect(m_windowHandle, &clientRect)) {
        throw GenericException(TEXT("GetClientRect"));
    }

    m_width = clientRect.right - clientRect.left;
    m_height = clientRect.bottom - clientRect.top;

    m_viewport.Width = static_cast<FLOAT>(m_width);
    m_viewport.Height = static_cast<FLOAT>(m_height);

    m_scissorRect.right = m_width;
    m_scissorRect.bottom = m_height;

    ReleaseBuffers();

    DXThrowIfFailed(m_swapChain->ResizeBuffers(
        FRAME_COUNT, m_width, m_height, DXGI_FORMAT_UNKNOWN,
        DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING
    ));

    GetBuffers(device);
}

void DXWindow::ConfineCursor()
{
    RECT clientRect;
    if (!GetClientRect(m_windowHandle, &clientRect)) {
        throw GenericException(TEXT("GetClientRect"));
    }

    POINT topLeft{clientRect.left, clientRect.top};
    POINT bottomRight{clientRect.right, clientRect.bottom};

    if (!ClientToScreen(m_windowHandle, &topLeft) || !ClientToScreen(m_windowHandle, &bottomRight)) {
        throw GenericException(TEXT("ClientToScreen"));
    }

    clientRect.left = topLeft.x;
    clientRect.top = topLeft.y;
    clientRect.right = bottomRight.x;
    clientRect.bottom = bottomRight.y;

    if (!ClipCursor(&clientRect)) {
        throw GenericException(TEXT("ClipCursor"));
    }
}

void DXWindow::FreeCursor()
{
    if (!ClipCursor(nullptr)) {
        throw GenericException(TEXT("ClipCursor"));
    }
}

void DXWindow::CenterCursor()
{
    RECT clientRect;
    if (!GetClientRect(m_windowHandle, &clientRect)) {
        throw GenericException(TEXT("GetClientRect"));
    }

    POINT center{
        .x = clientRect.left + (clientRect.right - clientRect.left) / 2,
        .y = clientRect.top + (clientRect.bottom - clientRect.top) / 2
    };

    if (!ClientToScreen(m_windowHandle, &center)) {
        throw GenericException(TEXT("ClientToScreen"));
    }

    if (!SetCursorPos(center.x, center.y)) {
        throw GenericException(TEXT("SetCursorPos"));
    }
}

void DXWindow::SetCursorVisibility(BOOL enable)
{
    if (m_cursorVisible != enable) {
        ShowCursor(enable);
        m_cursorVisible = enable;
    }
}

void DXWindow::SetFullscreen(BOOL enable)
{
    DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
    DWORD exStyle = WS_EX_OVERLAPPEDWINDOW | WS_EX_APPWINDOW;

    if (enable) {
        style = WS_POPUP | WS_VISIBLE;
        exStyle = WS_EX_APPWINDOW;
    }

    SetWindowLongPtr(m_windowHandle, GWL_STYLE, style);
    SetWindowLongPtr(m_windowHandle, GWL_EXSTYLE, exStyle);

    m_isFullscreen = enable;

    if (enable) {
        HMONITOR monitor = MonitorFromWindow(m_windowHandle, MONITOR_DEFAULTTONEAREST);
        MONITORINFO monitorInfo{.cbSize = sizeof(monitorInfo)};
        if (!GetMonitorInfo(monitor, &monitorInfo)) {
            throw GenericException(TEXT("GetMonitorInfo"));
        }

        if (!SetWindowPos(
                m_windowHandle, nullptr, monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top,
                monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
                monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top, SWP_NOZORDER
            )) {
            throw GenericException(TEXT("SetWindowPos"));
        }
    }
    else {
        ShowWindow(m_windowHandle, SW_MAXIMIZE);
    }
}

POINT DXWindow::GetCursorPosition() const
{
    POINT cursorPos;
    if (!GetCursorPos(&cursorPos)) {
        throw GenericException(TEXT("GetCursorPos"));
    }

    if (!ScreenToClient(m_windowHandle, &cursorPos)) {
        throw GenericException(TEXT("ScreenToClient"));
    }

    return cursorPos;
}

void DXWindow::QueuePreRenderingTransitions(std::vector<D3D12_RESOURCE_BARRIER>& barriers) const
{
    QueueTransition(
        m_rtBuffers[m_swapChain->GetCurrentBackBufferIndex()].Get(), D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET, barriers
    );
}

void DXWindow::QueuePostRenderingTransitions(std::vector<D3D12_RESOURCE_BARRIER>& barriers) const
{
    QueueTransition(
        m_rtBuffers[m_swapChain->GetCurrentBackBufferIndex()].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT, barriers
    );
}

void DXWindow::GetBuffers(ID3D12Device10* device)
{
    for (UINT i = 0; i < FRAME_COUNT; i++) {
        DXThrowIfFailed(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_rtBuffers[i])));

        D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{
            .Format = RTV_FORMAT,
            .ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D,
            .Texture2D{.MipSlice = 0, .PlaneSlice = 0}
        };

        device->CreateRenderTargetView(m_rtBuffers[i].Get(), &rtvDesc, m_rtvHandles[i]);
    }

    m_dsBuffer = CreateDepthStencilBuffer(m_width, m_height, DSV_FORMAT, D3D12_RESOURCE_STATE_DEPTH_WRITE, device);

    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{
        .Format = DSV_FORMAT, .ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D, .Flags = D3D12_DSV_FLAG_NONE
    };

    device->CreateDepthStencilView(m_dsBuffer.Get(), &dsvDesc, m_dsvHandle);
}

void DXWindow::ReleaseBuffers()
{
    for (auto& buffer : m_rtBuffers) {
        buffer.Reset();
    }

    m_dsBuffer.Reset();
}

ATOM GetEventQueueWndClass(HINSTANCE instance)
{
    static ATOM atom = 0;
    static BOOL initialized = FALSE;

    if (initialized) {
        return atom;
    }

    WNDCLASSEX wcex{
        .cbSize = sizeof(wcex),
        .style = CS_OWNDC,
        .lpfnWndProc = EventQueueWindowProc,
        .cbClsExtra = 0,
        .cbWndExtra = sizeof(LONG_PTR),
        .hInstance = instance,
        .hIcon = LoadIcon(nullptr, IDI_APPLICATION),
        .hCursor = LoadCursor(nullptr, IDC_ARROW),
        .hbrBackground = nullptr,
        .lpszMenuName = nullptr,
        .lpszClassName = TEXT("EventQueueWndClass"),
        .hIconSm = LoadIcon(nullptr, IDI_APPLICATION)
    };

    atom = RegisterClassEx(&wcex);
    if (!atom) {
        throw GenericException(TEXT("RegisterClassEx"));
    }

    initialized = TRUE;

    return atom;
}

ATOM GetImGuiWndClass(HINSTANCE instance)
{
    static ATOM atom = 0;
    static BOOL initialized = FALSE;

    if (initialized) {
        return atom;
    }

    WNDCLASSEX wcex{
        .cbSize = sizeof(wcex),
        .style = CS_OWNDC,
        .lpfnWndProc = ImGuiWindowProc,
        .cbClsExtra = 0,
        .cbWndExtra = sizeof(LONG_PTR),
        .hInstance = instance,
        .hIcon = LoadIcon(nullptr, IDI_APPLICATION),
        .hCursor = LoadCursor(nullptr, IDC_ARROW),
        .hbrBackground = nullptr,
        .lpszMenuName = nullptr,
        .lpszClassName = TEXT("ImGuiWndClass"),
        .hIconSm = LoadIcon(nullptr, IDI_APPLICATION)
    };

    atom = RegisterClassEx(&wcex);
    if (!atom) {
        throw GenericException(TEXT("RegisterClassEx"));
    }

    initialized = TRUE;

    return atom;
}

void UnregisterWndClasses(HINSTANCE instance)
{
    if (!UnregisterClass(MAKEINTATOM(GetEventQueueWndClass(instance)), instance)) {
        throw GenericException(TEXT("UnregisterClass"));
    }

    if (!UnregisterClass(MAKEINTATOM(GetImGuiWndClass(instance)), instance)) {
        throw GenericException(TEXT("UnregisterClass"));
    }
}
