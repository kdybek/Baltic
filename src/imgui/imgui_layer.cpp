#include "imgui_layer.hpp"

#include "imgui.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK ImGuiWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
    {
        return true;
    }

    return BalticWindowProc(hWnd, msg, wParam, lParam);
}

ImGuiLayer::ImGuiLayer(HINSTANCE instance) : ImGuiWindowClass(instance, TEXT("ImGuiWindowClass"), ImGuiWindowProc)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();
}

ImGuiLayer::~ImGuiLayer()
{
    ImGui::DestroyContext();
}
