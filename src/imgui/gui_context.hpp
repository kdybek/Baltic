#pragma once

// clang-format off
#include "auxiliary/pch.hpp"
// clang-format on

#include "auxiliary/types.hpp"
#include "imgui.h"

class GUIContext
{
public:
    GUIContext(HWND windowHandle, ID3D12Device10* device);
    ~GUIContext();

    GUIContext(const GUIContext&) = delete;
    GUIContext& operator=(const GUIContext&) = delete;

    void BeginFrame();
    void QueueDraw(ID3D12GraphicsCommandList* cmdList);

private:
    ImGuiContext* m_imguiContext;
    ComPtr<ID3D12DescriptorHeap> m_srvHeap;
};

LRESULT CALLBACK GUIMsgQueueWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
