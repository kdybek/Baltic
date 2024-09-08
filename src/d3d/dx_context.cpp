#include "dx_context.h"

#include "auxiliary/baltic_exception.h"
#include "auxiliary/constants.h"

namespace Baltic
{
    DXContext::DXContext() : m_fenceValue(0), m_fenceEvent(nullptr)
    {
        ThrowIfFailed(CreateDXGIFactory2(0, IID_PPV_ARGS(&m_factory)));

        ThrowIfFailed(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)));

        D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{
            .Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
            .Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH,
            .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
            .NodeMask = 0
        };

        ThrowIfFailed(m_device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&m_cmdQueue)));

        ThrowIfFailed(m_device->CreateFence(m_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));

        if (!(m_fenceEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr))) {
            throw BalticException("CreateEventW");
        }

        ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_cmdAllocator)));

        ThrowIfFailed(m_device->CreateCommandList1(
            0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&m_cmdList)
        ));
    }

    DXContext::~DXContext()
    {
        if (m_fenceEvent) {
            CloseHandle(m_fenceEvent);
        }
    }

    void DXContext::SignalAndWait()
    {
        ThrowIfFailed(m_cmdQueue->Signal(m_fence.Get(), ++m_fenceValue));

        ThrowIfFailed(m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent));

        if (WaitForSingleObject(m_fenceEvent, EVENT_TIMEOUT) != WAIT_OBJECT_0) {
            throw BalticException("WaitForSingleObject");
        }
    }

    void DXContext::ResetCmdList()
    {
        ThrowIfFailed(m_cmdAllocator->Reset());

        ThrowIfFailed(m_cmdList->Reset(m_cmdAllocator.Get(), nullptr));
    }

    void DXContext::ExecuteCmdList()
    {
        ThrowIfFailed(m_cmdList->Close());

        ID3D12CommandList* lists[]{m_cmdList.Get()};
        m_cmdQueue->ExecuteCommandLists(1, lists);
        SignalAndWait();
    }

    void DXContext::Flush(UINT count)
    {
        for (UINT i = 0; i < count; i++) {
            SignalAndWait();
        }
    }

} // namespace Baltic
