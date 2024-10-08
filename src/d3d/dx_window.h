#pragma once

// clang-format off
#include "auxiliary/pch.h"
// clang-format on

#include <mutex>
#include <queue>

#include "auxiliary/constants.h"
#include "auxiliary/types.h"
#include "d3d/dx_context.h"

class DXWindow
{
public:
    DXWindow(UINT width, UINT height, DXContext& context);
    ~DXWindow();

    DXWindow(const DXWindow&) = delete;
    DXWindow& operator=(const DXWindow&) = delete;

    void Update();
    void Present();
    [[nodiscard]] Event PollEvent();
    void Resize(ID3D12Device10* device);
    void ConfineCursor();
    void CenterCursor();
    [[nodiscard]] POINT GetCursorPosition();
    void SetFullscreen(BOOL enable);
    void QueuePreRenderingTransitions(std::vector<D3D12_RESOURCE_BARRIER>& barriers);
    void QueuePostRenderingTransitions(std::vector<D3D12_RESOURCE_BARRIER>& barriers);

    [[nodiscard]] inline UINT GetWidth() const { return m_width; }
    [[nodiscard]] inline UINT GetHeight() const { return m_height; }
    [[nodiscard]] inline FLOAT GetAspectRatio() const { return static_cast<FLOAT>(m_width) / m_height; }
    [[nodiscard]] inline const D3D12_VIEWPORT* GetViewportPtr() const { return &m_viewport; }
    [[nodiscard]] inline const D3D12_RECT* GetScissorRectPtr() const { return &m_scissorRect; }
    [[nodiscard]] inline BOOL isFullscreen() const { return m_isFullscreen; }
    [[nodiscard]] inline const D3D12_CPU_DESCRIPTOR_HANDLE* GetBackBufferRTVHandlePtr() const
    {
        return &m_rtvHandles[m_swapChain->GetCurrentBackBufferIndex()];
    }
    [[nodiscard]] inline const D3D12_CPU_DESCRIPTOR_HANDLE* GetDSVHandlePtr() const { return &m_dsvHandle; }

private:
    friend LRESULT CALLBACK OnWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);

    void GetBuffers(ID3D12Device10* device);
    void ReleaseBuffers();

private:
    ATOM m_wndClass;
    HWND m_windowHandle;
    UINT m_width;
    UINT m_height;
    D3D12_VIEWPORT m_viewport;
    D3D12_RECT m_scissorRect;
    BOOL m_isFullscreen;

    std::mutex m_eventQueueMutex;
    std::queue<Event> m_eventQueue;

    ComPtr<IDXGISwapChain4> m_swapChain;
    ComPtr<ID3D12Resource2> m_rtBuffers[FRAME_COUNT];
    ComPtr<ID3D12DescriptorHeap> m_rtvDescHeap;
    D3D12_CPU_DESCRIPTOR_HANDLE m_rtvHandles[FRAME_COUNT];

    ComPtr<ID3D12Resource2> m_dsBuffer;
    ComPtr<ID3D12DescriptorHeap> m_dsvHeapDesc;
    D3D12_CPU_DESCRIPTOR_HANDLE m_dsvHandle;
};
