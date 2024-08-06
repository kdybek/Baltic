#include "dx_window.h"

#include "auxiliary/baltic_except.h"
#include "auxiliary/constants.h"

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

    DXWindow::DXWindow()
            : m_wndClass(0),
              m_window(nullptr),
              m_width(1920),
              m_height(1080),
              m_shouldClose(FALSE),
              m_shouldResize(FALSE),
              m_isFullscreen(FALSE)
    {
        WNDCLASSEXW wcex{
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
                WS_EX_OVERLAPPEDWINDOW | WS_EX_APPWINDOW,
                reinterpret_cast<LPCWSTR>(m_wndClass),
                L"Baltic",
                WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                100,
                100,
                1920,
                1080,
                nullptr,
                nullptr,
                wcex.hInstance,
                this
        ))) {
            throw BalticException("CreateWindowExW");
        }

        const auto& factory = DXContext::Get().GetFactory();

        DXGI_SWAP_CHAIN_DESC1 swapChainDesc{
                .Width = 1920,
                .Height = 1080,
                .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
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

        Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;

        if (FAILED(factory->CreateSwapChainForHwnd(
                DXContext::Get().GetCmdQueue().Get(),
                m_window,
                &swapChainDesc,
                &swapChainFullscreenDesc,
                nullptr,
                &swapChain1
        ))) {
            throw BalticException("factory->CreateSwapChainForHwnd");
        }

        if (FAILED(swapChain1.As(&m_swapChain))) {
            throw BalticException("swapChain1.As");
        }
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

    void DXWindow::ResizeSwapChain()
    {
        RECT clientRect;
        if (!GetClientRect(m_window, &clientRect)) {
            throw BalticException("GetClientRect");
        }

        m_width = clientRect.right - clientRect.left;
        m_height = clientRect.bottom - clientRect.top;

        if (FAILED(m_swapChain->ResizeBuffers(
                FRAME_COUNT,
                m_width,
                m_height,
                DXGI_FORMAT_UNKNOWN,
                DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING
        ))) {
            throw BalticException("m_swapChain->ResizeBuffers");
        }

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
            MONITORINFO monitorInfo{.cbSize = sizeof(monitorInfo)};
            if (!GetMonitorInfoW(monitor, &monitorInfo)) {
                throw BalticException("GetMonitorInfoW");
            }

            if (!SetWindowPos(
                    m_window,
                    nullptr,
                    monitorInfo.rcMonitor.left,
                    monitorInfo.rcMonitor.top,
                    monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
                    monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
                    SWP_NOZORDER
            )) {
                throw BalticException("SetWindowPos");
            }
        }
        else {
            ShowWindow(m_window, SW_MAXIMIZE);
        }
    }

} // Baltic