#include "d3d/buffers.h"

#include "auxiliary/baltic_except.h"

namespace Baltic
{
    UploadBuffer::UploadBuffer(UINT64 size, ID3D12Device5* device)
    {
        D3D12_HEAP_PROPERTIES heapPropertiesUpload = {
            .Type = D3D12_HEAP_TYPE_UPLOAD,
            .CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
            .MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
            .CreationNodeMask = 0,
            .VisibleNodeMask = 0
        };

        D3D12_RESOURCE_DESC resourceDesc = {
            .Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
            .Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT,
            .Width = size,
            .Height = 1,
            .DepthOrArraySize = 1,
            .MipLevels = 1,
            .Format = DXGI_FORMAT_UNKNOWN,
            .SampleDesc = {.Count = 1, .Quality = 0},
            .Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
            .Flags = D3D12_RESOURCE_FLAG_NONE
        };

        if (FAILED(device->CreateCommittedResource(
                &heapPropertiesUpload, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                IID_PPV_ARGS(&m_uploadBuffer)
            ))) {
            throw BalticException("CreateCommittedResource");
        }
    }

    void UploadBuffer::CopyData(const void* data, UINT size) const
    {
        void* mappedData;
        if (FAILED(m_uploadBuffer->Map(0, nullptr, &mappedData))) {
            throw BalticException("Map");
        }

        memcpy(mappedData, data, size);

        m_uploadBuffer->Unmap(0, nullptr);
    }

    void UploadBuffer::StageCmdUpload(ID3D12Resource2* dest, UINT size, ID3D12GraphicsCommandList6* commandList) const
    {
        commandList->CopyBufferRegion(dest, 0, m_uploadBuffer.Get(), 0, size);
    }

    VertexBuffer::VertexBuffer(UINT64 size, ID3D12Device5* device)
    {
        D3D12_HEAP_PROPERTIES heapPropertiesUpload = {
            .Type = D3D12_HEAP_TYPE_DEFAULT,
            .CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
            .MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
            .CreationNodeMask = 0,
            .VisibleNodeMask = 0
        };

        D3D12_RESOURCE_DESC resourceDesc = {
            .Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
            .Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT,
            .Width = size,
            .Height = 1,
            .DepthOrArraySize = 1,
            .MipLevels = 1,
            .Format = DXGI_FORMAT_UNKNOWN,
            .SampleDesc = {.Count = 1, .Quality = 0},
            .Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
            .Flags = D3D12_RESOURCE_FLAG_NONE
        };

        if (FAILED(device->CreateCommittedResource(
                &heapPropertiesUpload, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr,
                IID_PPV_ARGS(&m_vertexBuffer)
            ))) {
            throw BalticException("CreateCommittedResource");
        }
    }

} // namespace Baltic
