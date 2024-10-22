#pragma once

// clang-format off
#include "auxiliary/pch.hpp"
// clang-format on

#include "auxiliary/constants.hpp"
#include "auxiliary/types.hpp"
#include "d3d/dx_context.hpp"

class WindowClass
{
public:
    WindowClass(HINSTANCE instance, const TCHAR* className, WNDPROC wndProc);
    ~WindowClass();

    WindowClass(const WindowClass&) = delete;
    WindowClass& operator=(const WindowClass&) = delete;

    [[nodiscard]] inline ATOM GetAtom() const { return m_atom; }

private:
    ATOM m_atom;
    HINSTANCE m_instance;
};

class DXWindow
{
public:
    DXWindow(HINSTANCE instance, ATOM wndClass, const TCHAR* wndName, UINT width, UINT height, DXContext& context);
    ~DXWindow();

    DXWindow(const DXWindow&) = delete;
    DXWindow& operator=(const DXWindow&) = delete;

    void Update();
    void Present();
    [[nodiscard]] WindowsMessage PollMsg();
    void Resize(ID3D12Device10* device);
    void ConfineCursor();
    void FreeCursor();
    void CenterCursor();
    void SetCursorVisibility(BOOL enable);
    void SetFullscreen(BOOL enable);

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
    [[nodiscard]] inline HWND GetWindowHandle() const { return m_windowHandle; }
    [[nodiscard]] inline DirectX::XMMATRIX GetProjectionMatrix() const
    {
        return DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(m_fov), GetAspectRatio(), m_nearZ, m_farZ);
    }
    [[nodiscard]] POINT GetCursorPosition() const;
    void QueuePreRenderingTransitions(std::vector<D3D12_RESOURCE_BARRIER>& barriers) const;
    void QueuePostRenderingTransitions(std::vector<D3D12_RESOURCE_BARRIER>& barriers) const;

private:
    friend LRESULT CALLBACK MsgQueueWindowProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);

    void GetBuffers(ID3D12Device10* device);
    void ReleaseBuffers();

private:
    HWND m_windowHandle;
    UINT m_width;
    UINT m_height;
    D3D12_VIEWPORT m_viewport;
    D3D12_RECT m_scissorRect;
    BOOL m_isFullscreen;
    BOOL m_cursorVisible;
    FLOAT m_fov;
    FLOAT m_nearZ;
    FLOAT m_farZ;

    std::queue<WindowsMessage> m_messageQueue;

    ComPtr<IDXGISwapChain4> m_swapChain;
    ComPtr<ID3D12DescriptorHeap> m_rtvDescHeap;
    D3D12_CPU_DESCRIPTOR_HANDLE m_rtvHandles[FRAME_COUNT];
    ComPtr<ID3D12Resource2> m_rtBuffers[FRAME_COUNT];
    ComPtr<ID3D12DescriptorHeap> m_dsvDescHeap;
    D3D12_CPU_DESCRIPTOR_HANDLE m_dsvHandle;
    ComPtr<ID3D12Resource2> m_dsBuffer;
};

LRESULT CALLBACK MsgQueueWindowProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);
