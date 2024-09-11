#include "d3d/dx_resource.h"

#include "auxiliary/baltic_exception.h"

namespace Baltic
{
    DXResource::DXResource(
        D3D12_HEAP_PROPERTIES heapProperties, D3D12_RESOURCE_DESC resourceDesc, D3D12_RESOURCE_STATES resourceState,
        ID3D12Device5* device
    )
        : m_resourceState(resourceState)
    {
        DXThrowIfFailed(device->CreateCommittedResource(
            &heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, resourceState, nullptr, IID_PPV_ARGS(&m_resource)
        ));
    }

    void DXResource::CopyData(const void* data, UINT64 size, UINT64 offset)
    {
        void* mappedData;
        DXThrowIfFailed(m_resource->Map(0, nullptr, &mappedData));
        std::memcpy(static_cast<BYTE*>(mappedData) + offset, data, size);
        m_resource->Unmap(0, nullptr);
    }

    DXResource CreateUploadBuffer(UINT64 size, ID3D12Device5* device, D3D12_RESOURCE_STATES resourceState)
    {
        D3D12_HEAP_PROPERTIES heapPropertiesUpload{
            .Type = D3D12_HEAP_TYPE_UPLOAD,
            .CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
            .MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
            .CreationNodeMask = 0,
            .VisibleNodeMask = 0
        };

        D3D12_RESOURCE_DESC resourceDesc{
            .Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
            .Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT,
            .Width = size,
            .Height = 1,
            .DepthOrArraySize = 1,
            .MipLevels = 1,
            .Format = DXGI_FORMAT_UNKNOWN,
            .SampleDesc{.Count = 1, .Quality = 0},
            .Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
            .Flags = D3D12_RESOURCE_FLAG_NONE
        };

        return DXResource(heapPropertiesUpload, resourceDesc, resourceState, device);
    }

    DXResource CreateGPUBuffer(UINT64 size, ID3D12Device5* device, D3D12_RESOURCE_STATES resourceState)
    {
        D3D12_HEAP_PROPERTIES heapPropertiesUpload{
            .Type = D3D12_HEAP_TYPE_DEFAULT,
            .CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
            .MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
            .CreationNodeMask = 0,
            .VisibleNodeMask = 0
        };

        D3D12_RESOURCE_DESC resourceDesc{
            .Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
            .Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT,
            .Width = size,
            .Height = 1,
            .DepthOrArraySize = 1,
            .MipLevels = 1,
            .Format = DXGI_FORMAT_UNKNOWN,
            .SampleDesc{.Count = 1, .Quality = 0},
            .Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
            .Flags = D3D12_RESOURCE_FLAG_NONE
        };

        return DXResource(heapPropertiesUpload, resourceDesc, resourceState, device);
    }

} // namespace Baltic
