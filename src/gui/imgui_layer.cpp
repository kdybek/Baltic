#include "imgui_layer.hpp"

#include "auxiliary/baltic_exception.hpp"
#include "backends/imgui_impl_dx12.h"
#include "backends/imgui_impl_win32.h"
#include "imgui.h"

GUI::GUI(DXWindow window, ID3D12Device10* device) : m_window(std::move(window))
{
    OutputDebugString(TEXT("GUI created\n"));
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

GUI::~GUI()
{
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK ImGuiWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) {
        return TRUE;
    }

    return BalticWindowProc(hWnd, msg, wParam, lParam);
}

ImGuiLayer::ImGuiLayer(HINSTANCE instance) : m_imGuiWndClass(instance, TEXT("ImGuiWndClass"), ImGuiWindowProc)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();
}

ImGuiLayer::~ImGuiLayer() { ImGui::DestroyContext(); }

GUI ImGuiLayer::CreateGUI(HINSTANCE instance, const TCHAR* wndName, UINT width, UINT height, DXContext& dxContext)
{
    return GUI(DXWindow(instance, m_imGuiWndClass.GetAtom(), wndName, width, height, dxContext), dxContext.GetDeviceComPtr().Get());
}
