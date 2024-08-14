#include "dx_context.h"

#include "auxiliary/baltic_except.h"
#include "auxiliary/constants.h"

namespace Baltic
{
    DXContext::DXContext() : m_fenceValue(0), m_fenceEvent(nullptr)
    {
        if (FAILED(CreateDXGIFactory2(0, IID_PPV_ARGS(&m_factory)))) {
            throw BalticException("CreateDXGIFactory2");
        }

        if (FAILED(D3D12CreateDevice(
                nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)
            ))) {
            throw BalticException("D3D12CreateDevice");
        }

        D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{
            .Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
            .Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH,
            .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
            .NodeMask = 0
        };

        if (FAILED(m_device->CreateCommandQueue(
                &cmdQueueDesc, IID_PPV_ARGS(&m_cmdQueue)
            ))) {
            throw BalticException("m_device->CreateCommandQueue");
        }

        if (FAILED(m_device->CreateFence(
                m_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)
            ))) {
            throw BalticException("m_device->CreateFence");
        }

        if (!(m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr))) {
            throw BalticException("CreateEvent");
        }

        if (FAILED(m_device->CreateCommandAllocator(
                D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_cmdAllocator)
            ))) {
            throw BalticException("m_device->CreateCommandAllocator");
        }

        if (FAILED(m_device->CreateCommandList1(
                0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE,
                IID_PPV_ARGS(&m_cmdList)
            ))) {
            throw BalticException("m_device->CreateCommandList1");
        }
    }

    DXContext::~DXContext()
    {
        if (m_fenceEvent) {
            CloseHandle(m_fenceEvent);
        }
    }

    void DXContext::SignalAndWait()
    {
        if (FAILED(m_cmdQueue->Signal(m_fence.Get(), ++m_fenceValue))) {
            throw BalticException("m_cmdQueue->Signal");
        }

        if (FAILED(m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent))) {
            throw BalticException("m_fence->SetEventOnCompletion");
        }

        if (WaitForSingleObject(m_fenceEvent, EVENT_TIMEOUT) != WAIT_OBJECT_0) {
            throw BalticException("WaitForSingleObject");
        }
    }

    void DXContext::ResetCmdList()
    {
        if (FAILED(m_cmdAllocator->Reset())) {
            throw BalticException("m_cmdAllocator->Reset");
        }

        if (FAILED(m_cmdList->Reset(m_cmdAllocator.Get(), nullptr))) {
            throw BalticException("m_cmdList->Reset");
        }
    }

    void DXContext::ExecuteCmdList()
    {
        if (FAILED(m_cmdList->Close())) {
            throw BalticException("m_cmdList->Close");
        }

        ID3D12CommandList* lists[] = {m_cmdList.Get()};
        m_cmdQueue->ExecuteCommandLists(1, lists);
        SignalAndWait();
    }

    void DXContext::Flush(UINT count)
    {
        for (UINT i = 0; i < count; i++) {
            SignalAndWait();
        }
    }

}  // namespace Baltic
