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

        [[nodiscard]] bool ShouldClose() const;

    private:
        static LRESULT CALLBACK OnWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        ATOM m_wndClass;
        HWND m_window;

        Microsoft::WRL::ComPtr<IDXGISwapChain4> m_swapChain;

    };

} // Baltic
