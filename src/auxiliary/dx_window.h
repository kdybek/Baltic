#pragma once

#include "win_include.h"

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

        [[nodiscard]] bool ShouldClose() const;

    private:
        static LRESULT CALLBACK OnWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        ATOM m_wndClass;
        HWND m_window;

    };

} // Baltic
