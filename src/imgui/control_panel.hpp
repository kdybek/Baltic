#pragma once

#include "auxiliary/baltic_exception.hpp"
#include "backends/imgui_impl_dx12.h"
#include "backends/imgui_impl_win32.h"
#include "d3d/dx_window.hpp"

class ControlPanel
{
public:
    inline ControlPanel(DXWindow window, ID3D12Device10* device) : m_window(std::move(window))
    {
        D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{
            .Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
            .NumDescriptors = 1,
            .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
            .NodeMask = 0
        };
        DXThrowIfFailed(device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_srvHeap)));

        ImGui_ImplWin32_Init(m_window.GetWindowHandle());
        ImGui_ImplDX12_Init(
            device, FRAME_COUNT, DXGI_FORMAT_R8G8B8A8_UNORM, m_srvHeap.Get(),
            m_srvHeap->GetCPUDescriptorHandleForHeapStart(), m_srvHeap->GetGPUDescriptorHandleForHeapStart()
        );
    }

    inline ~ControlPanel()
    {
        ImGui_ImplDX12_Shutdown();
        ImGui_ImplWin32_Shutdown();
    }

    ControlPanel(const ControlPanel&) = delete;
    ControlPanel& operator=(const ControlPanel&) = delete;

    [[nodiscard]] inline DXWindow& GetWindow() { return m_window; }

private:
    DXWindow m_window;
    ComPtr<ID3D12DescriptorHeap> m_srvHeap;
};
