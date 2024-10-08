#pragma once

// clang-format off
#include "auxiliary/pch.h"
// clang-format on

#include "auxiliary/types.h"

ComPtr<ID3D12Resource2> CreateUploadBuffer(UINT64 size, ID3D12Device5* device);
ComPtr<ID3D12Resource2> CreateGPUBuffer(UINT64 size, D3D12_RESOURCE_STATES resourceState, ID3D12Device5* device);
ComPtr<ID3D12Resource2> CreateDepthStencilBuffer(
    UINT width, UINT height, DXGI_FORMAT format, D3D12_RESOURCE_STATES resourceState, ID3D12Device5* device
);

void CopyDataToResource(ID3D12Resource2* resource, const void* data, UINT64 size, UINT64 offset = 0);
void QueueTransition(
    ID3D12Resource2* resource, D3D12_RESOURCE_STATES oldState, D3D12_RESOURCE_STATES newState,
    std::vector<D3D12_RESOURCE_BARRIER>& barriers
);
