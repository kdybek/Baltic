#pragma once

// clang-format off
#include "auxiliary/pch.h"
// clang-format on

#include "auxiliary/types.h"

namespace Baltic
{
    struct DXResource
    {
        ComPtr<ID3D12Resource2> resourceComPtr;
        D3D12_RESOURCE_STATES resourceState;
    };

    void CopyDataToResource(const DXResource& resource, const void* data, UINT64 size, UINT64 offset = 0);
    void QueueTransition(const DXResource& resource, D3D12_RESOURCE_STATES newState, std::vector<D3D12_RESOURCE_BARRIER>& barriers);

    DXResource CreateUploadBuffer(UINT64 size, ID3D12Device5* device);
    DXResource CreateGPUBuffer(UINT64 size, D3D12_RESOURCE_STATES resourceState, ID3D12Device5* device);
    DXResource CreateDepthStencilBuffer(
        UINT width, UINT height, DXGI_FORMAT format, D3D12_RESOURCE_STATES resourceState, ID3D12Device5* device
    );

    void StageCmdResourceBarrier(
        ID3D12GraphicsCommandList6* commandList, const std::vector<D3D12_RESOURCE_BARRIER>& barriers
    );

} // namespace Baltic
