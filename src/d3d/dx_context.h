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

        [[nodiscard]] inline const DXDevice8ComPtr& GetDevice() const { return m_device; }

        [[nodiscard]] inline const DXCommandQueueComPtr& GetCmdQueue() const { return m_cmdQueue; }

        [[nodiscard]] inline const DXGraphicsCommandList6ComPtr& GetCmdList() const { return m_cmdList; }

    private:
        DXGIFactory7ComPtr m_factory;

        DXDevice8ComPtr m_device;
        DXCommandQueueComPtr m_cmdQueue;

        DXCommandAllocatorComPtr m_cmdAllocator;
        DXGraphicsCommandList6ComPtr m_cmdList;

        DXFence1ComPtr m_fence;
        UINT64 m_fenceValue;
        HANDLE m_fenceEvent;
    };

}  // namespace Baltic
