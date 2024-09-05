#pragma once

#include "auxiliary/types.h"

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

        [[nodiscard]] inline const DXGIFactory7ComPtr& GetFactory() const { return m_factory; }

        [[nodiscard]] inline const D3D12Device8ComPtr& GetDevice() const { return m_device; }

        [[nodiscard]] inline const D3D12CommandQueueComPtr& GetCmdQueue() const { return m_cmdQueue; }

        [[nodiscard]] inline const D3D12GraphicsCommandList6ComPtr& GetCmdList() const { return m_cmdList; }

    private:
        DXGIFactory7ComPtr m_factory;

        D3D12Device8ComPtr m_device;
        D3D12CommandQueueComPtr m_cmdQueue;

        D3D12CommandAllocatorComPtr m_cmdAllocator;
        D3D12GraphicsCommandList6ComPtr m_cmdList;

        D3D12Fence1ComPtr m_fence;
        UINT64 m_fenceValue;
        HANDLE m_fenceEvent;
    };

}  // namespace Baltic
