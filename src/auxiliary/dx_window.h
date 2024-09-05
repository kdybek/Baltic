#pragma once

#include "auxiliary/constants.h"
#include "d3d/dx_context.h"
#include "auxiliary/types.h"

namespace Baltic
{
    class DXWindow
    {
    public:
        DXWindow(UINT width, UINT height, const DXContext& dxContext);

        ~DXWindow();

        DXWindow(const DXWindow&) = delete;

        DXWindow& operator=(const DXWindow&) = delete;

        void Update();

        void Present();

        void ResizeSwapChain();

        void SetFullscreen(BOOL enable);

        void BeginFrame(ID3D12GraphicsCommandList6* cmdList);

        void EndFrame(ID3D12GraphicsCommandList6* cmdList);

        [[nodiscard]] inline UINT GetWidth() const { return m_width; }

        [[nodiscard]] inline UINT GetHeight() const { return m_height; }

        [[nodiscard]] inline BOOL ShouldClose() const { return m_shouldClose; }

        [[nodiscard]] inline BOOL ShouldResize() const { return m_shouldResize; }

        [[nodiscard]] inline BOOL isFullscreen() const { return m_isFullscreen; }

    private:
        friend LRESULT CALLBACK OnWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);

        void GetBuffers();

        void ReleaseBuffers();

    private:
        ATOM m_wndClass;
        HWND m_window;
        UINT m_width;
        UINT m_height;
        BOOL m_shouldClose;
        BOOL m_shouldResize;
        BOOL m_isFullscreen;

        DXSwapChain4ComPtr m_swapChain;
        DXResource2ComPtr m_buffers[FRAME_COUNT];
        UINT m_currentBufferIdx;

        DXDescriptorHeapComPtr m_rtvDescHeap;
        D3D12_CPU_DESCRIPTOR_HANDLE m_rtvHandles[FRAME_COUNT];

        const DXContext& m_dxContext;
    };

}  // namespace Baltic
