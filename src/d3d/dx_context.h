#pragma once

#include "auxiliary/win_include.h"

namespace Baltic
{
    class DXContext
    {
    public:
        DXContext();

        ~DXContext();

        DXContext(const DXContext&) = delete;

        DXContext& operator=(const DXContext&) = delete;

        void SignalAndWait();

        void ResetCmdList();

        void ExecuteCmdList();

        void Flush(UINT count);

        [[nodiscard]] inline const ComPtr<IDXGIFactory7>& GetFactoryComPtr() { return m_factory; }

        [[nodiscard]] inline const ComPtr<ID3D12Device8>& GetDeviceComPtr() { return m_device; }

        [[nodiscard]] inline const ComPtr<ID3D12CommandQueue>& GetCmdQueueComPtr() { return m_cmdQueue; }

        [[nodiscard]] inline const ComPtr<ID3D12GraphicsCommandList6>& GetCmdListComPtr() { return m_cmdList; }

    private:
        ComPtr<IDXGIFactory7> m_factory;

        ComPtr<ID3D12Device8> m_device;
        ComPtr<ID3D12CommandQueue> m_cmdQueue;

        ComPtr<ID3D12CommandAllocator> m_cmdAllocator;
        ComPtr<ID3D12GraphicsCommandList6> m_cmdList;

        ComPtr<ID3D12Fence1> m_fence;
        UINT64 m_fenceValue;
        HANDLE m_fenceEvent;
    };

}  // namespace Baltic
