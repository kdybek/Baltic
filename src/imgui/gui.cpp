#include "imgui/gui.hpp"

#include "auxiliary/baltic_exception.hpp"
#include "auxiliary/constants.hpp"
#include "backends/imgui_impl_dx12.h"
#include "backends/imgui_impl_win32.h"
#include "d3d/dx_window.hpp"

GUI::GUI(HWND windowHandle, ID3D12Device10* device)
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

GUI::~GUI()
{
    ImGui::SetCurrentContext(m_imguiContext);

    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext(m_imguiContext);
}

UINT GUI::AddWindow(const CHAR* name)
{
    m_windows.push_back({.name = name});
    return m_windows.size() - 1;
}

void GUI::AddSlider(UINT windowIdx, const CHAR* name, FLOAT* varPtr, FLOAT minVal, FLOAT maxVal)
{
    m_windows[windowIdx].elements.push_back(
        SliderData{.name = name, .varPtr = varPtr, .minVal = minVal, .maxVal = maxVal}
    );
}

void GUI::AddCheckbox(UINT windowIdx, const CHAR* name, BOOL* varPtr)
{
    m_windows[windowIdx].elements.push_back(CheckboxData{.name = name, .varPtr = varPtr});
}

namespace
{
    template <typename... Callable>
    struct Visitor : Callable...
    {
        using Callable::operator()...;
    };

} // namespace

void GUI::QueueDraw(ID3D12GraphicsCommandList* cmdList)
{
    ImGui::SetCurrentContext(m_imguiContext);

    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    for (const auto& window : m_windows) {
        ImGui::Begin(window.name);

        for (const auto& element : window.elements) {
            std::visit(
                Visitor{
                    [this](const SliderData& data) {
                        ImGui::SliderFloat(data.name, data.varPtr, data.minVal, data.maxVal);
                    },
                    [this](const CheckboxData& data) {
                        ImGui::Checkbox(data.name, reinterpret_cast<bool*>(data.varPtr));
                    }
                },
                element
            );
        }

        ImGui::End();
    }

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
