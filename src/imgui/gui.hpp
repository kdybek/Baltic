#pragma once

// clang-format off
#include "auxiliary/pch.hpp"
// clang-format on

#include "d3d/dx_window.hpp"

class GUI
{
public:
    GUI(HWND windowHandle, ID3D12Device10* device);
    ~GUI();

    GUI(const GUI&) = delete;
    GUI& operator=(const GUI&) = delete;

    void BeginFrame();
    void QueueDrawData(ID3D12GraphicsCommandList* commandList);
    [[nodiscard]] inline const ComPtr<ID3D12DescriptorHeap>& GetSRVHeapComPtr() { return m_srvHeap; }

private:
    ComPtr<ID3D12DescriptorHeap> m_srvHeap;
};

LRESULT CALLBACK GUIMsgQueueWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
