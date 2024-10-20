#pragma once

// clang-format off
#include "auxiliary/pch.hpp"
// clang-format on

#include "d3d/dx_window.hpp"

class ImGuiLayer
{
public:
    ImGuiLayer(HINSTANCE instance);
    ~ImGuiLayer();

    [[nodiscard]] inline const WindowClass& GetWindowClass() const { return ImGuiWindowClass; }

private:
    WindowClass ImGuiWindowClass;
};
