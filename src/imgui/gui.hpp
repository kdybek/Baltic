#pragma once

// clang-format off
#include "auxiliary/pch.hpp"
// clang-format on

#include "auxiliary/types.hpp"
#include "imgui.h"

class GUI
{
public:
    GUI(HWND windowHandle, ID3D12Device10* device);
    ~GUI();

    GUI(const GUI&) = delete;
    GUI& operator=(const GUI&) = delete;

    UINT AddWindow(const CHAR* name);
    void AddSlider(UINT guiWindowHandle, const CHAR* name, FLOAT* varPtr, FLOAT minVal, FLOAT maxVal);
    void AddCheckbox(UINT guiWindowHandle, const CHAR* name, BOOL* varPtr);
    void QueueDraw(ID3D12GraphicsCommandList* cmdList);

private:
    struct SliderData {
        const CHAR* name;
        FLOAT* varPtr;
        FLOAT minVal;
        FLOAT maxVal;
    };

    struct CheckboxData {
        const CHAR* name;
        BOOL* varPtr;
    };

    using WindowElement = std::variant<SliderData, CheckboxData>;

    struct WindowData {
        const CHAR* name;
        std::vector<WindowElement> elements;
    };

    std::vector<WindowData> m_windows;

    ImGuiContext* m_imguiContext;
    ComPtr<ID3D12DescriptorHeap> m_srvHeap;
};

LRESULT CALLBACK GUIMsgQueueWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
