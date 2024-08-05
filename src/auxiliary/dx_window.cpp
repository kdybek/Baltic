#include "dx_window.h"

#include "auxiliary/baltic_except.h"
#include "auxiliary/constants.h"

#define IDI_APPLICATION_W MAKEINTRESOURCEW(32512)
#define IDC_ARROW_W MAKEINTRESOURCEW(32512)

namespace Baltic
{
    DXWindow::DXWindow() : m_wndClass(0), m_window(nullptr)
    {
        WNDCLASSEXW wcex{
                .cbSize = sizeof(wcex),
                .style = CS_OWNDC,
                .lpfnWndProc = &DXWindow::OnWindowMessage,
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
                nullptr
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

        SetWindowLongPtr(m_window, GWLP_USERDATA, static_cast<LONG_PTR>(FALSE));
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

    bool DXWindow::ShouldClose() const
    {
        return static_cast<BOOL>(GetWindowLongPtrW(m_window, GWLP_USERDATA));
    }

    LRESULT DXWindow::OnWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg) {
            case WM_CLOSE:
                SetWindowLongPtrW(wnd, GWLP_USERDATA, static_cast<LONG_PTR>(TRUE));
                return 0;
        }

        return DefWindowProcW(wnd, msg, wParam, lParam);
    }

} // Baltic