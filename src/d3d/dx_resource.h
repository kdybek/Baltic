#pragma once

// clang-format off
#include "auxiliary/pch.h"
// clang-format on

#include "auxiliary/types.h"

namespace Baltic
{
    class DXResource
    {
    public:
        DXResource(
            D3D12_HEAP_PROPERTIES heapProperties, D3D12_RESOURCE_DESC resourceDesc, D3D12_RESOURCE_STATES resourceState,
            ID3D12Device5* device
        );
        ~DXResource() = default;

        DXResource(const DXResource&) = delete;
        DXResource& operator=(const DXResource&) = delete;

        void CopyData(const void* data, UINT64 size, UINT64 offset = 0);

        [[nodiscard]] inline const ComPtr<ID3D12Resource2>& GetComPtr() { return m_resource; }

    private:
        ComPtr<ID3D12Resource2> m_resource;
        D3D12_RESOURCE_STATES m_resourceState;
    };

    DXResource CreateUploadBuffer(UINT64 size, ID3D12Device5* device, D3D12_RESOURCE_STATES resourceState);
    DXResource CreateGPUBuffer(UINT64 size, ID3D12Device5* device, D3D12_RESOURCE_STATES resourceState);

} // namespace Baltic
