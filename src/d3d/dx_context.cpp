#include "dx_context.h"

namespace Baltic
{

    DXContext::DXContext() : m_fenceValue(0)
    {
        if (FAILED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)))) {
            throw BalticException("D3D12CreateDevice");
        }

        D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{
                .Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
                .Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH,
                .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
                .NodeMask = 0
        };

        if (FAILED(m_device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&m_cmdQueue)))) {
            throw BalticException("CreateCommandQueue");
        }

        if (FAILED(m_device->CreateFence(m_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)))) {
            throw BalticException("CreateFence");
        }
    }

} // Baltic