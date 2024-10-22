#include "imgui/gui_context.hpp"

#include "auxiliary/baltic_exception.hpp"
#include "auxiliary/constants.hpp"
#include "backends/imgui_impl_dx12.h"
#include "backends/imgui_impl_win32.h"
#include "d3d/dx_window.hpp"

GUIContext::GUIContext(HWND windowHandle, ID3D12Device10* device)
{
    IMGUI_CHECKVERSION();
    m_imguiContext = ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();

    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{
        .Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
        .NumDescriptors = 1,
        .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
        .NodeMask = 0
    };

    DXThrowIfFailed(device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_srvHeap)));

    ImGui_ImplWin32_Init(windowHandle);
    ImGui_ImplDX12_Init(
        device, FRAME_COUNT, DXGI_FORMAT_R8G8B8A8_UNORM, m_srvHeap.Get(),
        m_srvHeap->GetCPUDescriptorHandleForHeapStart(), m_srvHeap->GetGPUDescriptorHandleForHeapStart()
    );
}

GUIContext::~GUIContext()
{
    ImGui::SetCurrentContext(m_imguiContext);

    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext(m_imguiContext);
}

void GUIContext::BeginFrame()
{
    ImGui::SetCurrentContext(m_imguiContext);

    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void GUIContext::QueueDraw(ID3D12GraphicsCommandList* cmdList)
{
    ImGui::Render();

    ID3D12DescriptorHeap* descriptorHeaps[] = {m_srvHeap.Get()};
    cmdList->SetDescriptorHeaps(1, descriptorHeaps);

    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmdList);
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK GUIMsgQueueWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) {
        return TRUE;
    }

    return MsgQueueWindowProc(hWnd, msg, wParam, lParam);
}
