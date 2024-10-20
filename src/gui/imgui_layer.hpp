#pragma once

// clang-format off
#include "auxiliary/pch.hpp"
// clang-format on

#include "d3d/dx_window.hpp"

class GUI
{
public:
    ~GUI();

    GUI(const GUI&) = delete;
    GUI& operator=(const GUI&) = delete;

    [[nodiscard]] inline DXWindow& GetWindow() { return m_window; }

private:
    friend class ImGuiLayer;

    GUI(DXWindow window, ID3D12Device10* device);

private:
    DXWindow m_window;
    ComPtr<ID3D12DescriptorHeap> m_srvHeap;
};

class ImGuiLayer
{
public:
    ImGuiLayer(HINSTANCE instance);
    ~ImGuiLayer();

    ImGuiLayer(const ImGuiLayer&) = delete;
    ImGuiLayer& operator=(const ImGuiLayer&) = delete;

    GUI CreateGUI(HINSTANCE instance, const TCHAR* wndName, UINT width, UINT height, DXContext& dxContext);

private:
    WindowClass m_imGuiWndClass;
};
