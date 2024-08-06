#pragma once

#include "auxiliary/win_include.h"
#include "d3d/dx_context.h"

namespace Baltic
{
    class DXWindow
    {
    public:
        DXWindow();

        ~DXWindow();

        DXWindow(const DXWindow&) = delete;

        DXWindow& operator=(const DXWindow&) = delete;

        void Update();

        void Present();

        void ResizeSwapChain();

        void SetFullscreen(BOOL enable);

        [[nodiscard]] inline BOOL ShouldClose() const
        {
            return m_shouldClose;
        }

        [[nodiscard]] inline BOOL ShouldResize() const
        {
            return m_shouldResize;
        }

        [[nodiscard]] inline BOOL isFullscreen() const
        {
            return m_isFullscreen;
        }

        friend LRESULT CALLBACK OnWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        ATOM m_wndClass;
        HWND m_window;
        UINT m_width;
        UINT m_height;
        BOOL m_shouldClose;
        BOOL m_shouldResize;
        BOOL m_isFullscreen;

        Microsoft::WRL::ComPtr<IDXGISwapChain4> m_swapChain;

    };

} // Baltic
