#pragma once

// clang-format off
#include "auxiliary/pch.hpp"
// clang-format on

#include "d3d/dx_window.hpp"

class GUI
{
public:
    GUI(DXWindow window, ID3D12Device10* device);
    ~GUI();

    GUI(const GUI&) = delete;
    GUI& operator=(const GUI&) = delete;

    [[nodiscard]] inline DXWindow& GetWindow() { return m_window; }

private:
    DXWindow m_window;
    ComPtr<ID3D12DescriptorHeap> m_srvHeap;
};

LRESULT CALLBACK GUIWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
