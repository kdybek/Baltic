#pragma once

#include "auxiliary/constants.h"
#include "auxiliary/win_include.h"
#include "d3d/dx_context.h"

namespace Baltic
{
    class DXWindow
    {
    public:
        DXWindow(UINT width, UINT height, DXContext& context);

        ~DXWindow();

        DXWindow(const DXWindow&) = delete;

        DXWindow& operator=(const DXWindow&) = delete;

        void Update();

        void Present();

        void ResizeSwapChain(ID3D12Device8* device);

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

        void GetBuffers(ID3D12Device8* device);

        void ReleaseBuffers();

    private:
        ATOM m_wndClass;
        HWND m_window;
        UINT m_width;
        UINT m_height;
        BOOL m_shouldClose;
        BOOL m_shouldResize;
        BOOL m_isFullscreen;

        ComPtr<IDXGISwapChain4> m_swapChain;
        ComPtr<ID3D12Resource2> m_buffers[FRAME_COUNT];
        UINT m_currentBufferIdx;

        ComPtr<ID3D12DescriptorHeap> m_rtvDescHeap;
        D3D12_CPU_DESCRIPTOR_HANDLE m_rtvHandles[FRAME_COUNT];
    };

} // namespace Baltic
