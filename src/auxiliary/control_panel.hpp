#pragma once

// clang-format off
#include "auxiliary/pch.hpp"
// clang-format on

#include "d3d/dx_context.hpp"
#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"

class ControlPanel
{
public:
    ControlPanel(HWND windowHandle, DXContext& context);
    ~ControlPanel();

    ControlPanel(const ControlPanel&) = delete;
    ControlPanel& operator=(const ControlPanel&) = delete;

    void Update();
    void Render();
};
