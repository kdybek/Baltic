#pragma once

// clang-format off
#include "auxiliary/pch.hpp"
// clang-format on

#include "auxiliary/types.hpp"

class GUI
{
public:
    GUI(HWND windowHandle, ID3D12Device10* device);
    ~GUI();

    GUI(const GUI&) = delete;
    GUI& operator=(const GUI&) = delete;

    void QueueDraw(ID3D12GraphicsCommandList* cmdList);

private:
    ComPtr<ID3D12DescriptorHeap> m_srvHeap;
};

LRESULT CALLBACK GUIMsgQueueWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
