#pragma once

// clang-format off
#include "auxiliary/pch.hpp"
// clang-format on

#include "auxiliary/baltic_exception.hpp"
#include "backends/imgui_impl_dx12.h"
#include "backends/imgui_impl_win32.h"
#include "d3d/dx_context.hpp"
#include "d3d/dx_window.hpp"
#include "imgui.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class ControlPanel
{
public:
    inline ControlPanel(HINSTANCE instance, DXContext& context)
        : m_window(instance, GetImGuiWndClass(instance), TEXT("Control Panel"), 1920, 1080, context)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        ImGui::StyleColorsDark();

        D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{
            .Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
            .NumDescriptors = 1,
            .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
            .NodeMask = 0
        };
        DXThrowIfFailed(context.GetDeviceComPtr()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_srvHeap)));

        ImGui_ImplWin32_Init(m_window.GetWindowHandle());
        ImGui_ImplDX12_Init(
            context.GetDeviceComPtr().Get(), FRAME_COUNT, DXGI_FORMAT_R8G8B8A8_UNORM, m_srvHeap.Get(),
            m_srvHeap->GetCPUDescriptorHandleForHeapStart(), m_srvHeap->GetGPUDescriptorHandleForHeapStart()
        );
    }

    inline ~ControlPanel()
    {
        ImGui_ImplDX12_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }

    ControlPanel(const ControlPanel&) = delete;
    ControlPanel& operator=(const ControlPanel&) = delete;

    inline void Update() { m_window.Update(); }
    inline void Present() { m_window.Present(); }

private:
    DXWindow m_window;
    ComPtr<ID3D12DescriptorHeap> m_srvHeap;
};
