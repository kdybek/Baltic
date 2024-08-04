#include "dx_window.h"

#include "baltic_except.h"

#define IDI_APPLICATIONW MAKEINTRESOURCEW(32512)
#define IDC_ARROWW MAKEINTRESOURCEW(32512)

namespace Baltic
{
    DXWindow::DXWindow() : m_wndClass(0), m_window(nullptr)
    {
        WNDCLASSEXW wcex{
                .cbSize = sizeof(wcex),
                .style = CS_OWNDC,
                .lpfnWndProc = &DXWindow::OnWindowMessage,
                .cbClsExtra = 0,
                .cbWndExtra = sizeof(LONG_PTR), // To communicate that the window closed from the callback function.
                .hInstance = GetModuleHandleW(nullptr),
                .hIcon = LoadIconW(nullptr, IDI_APPLICATIONW),
                .hCursor = LoadCursorW(nullptr, IDC_ARROWW),
                .hbrBackground = nullptr,
                .lpszMenuName = nullptr,
                .lpszClassName = L"BalticWndCls",
                .hIconSm = LoadIconW(nullptr, IDI_APPLICATIONW)
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

        SetWindowLongPtr(m_window, GWLP_USERDATA, static_cast<LONG_PTR>(false));
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

    bool DXWindow::ShouldClose() const
    {
        return static_cast<bool>(GetWindowLongPtrW(m_window, GWLP_USERDATA));
    }

    LRESULT DXWindow::OnWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg) {
            case WM_CLOSE:
                SetWindowLongPtrW(wnd, GWLP_USERDATA, static_cast<LONG_PTR>(true));
                return 0;
        }

        return DefWindowProcW(wnd, msg, wParam, lParam);
    }

} // Baltic