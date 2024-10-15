#include "d3d/dx_resource.hpp"

#include "auxiliary/baltic_exception.hpp"

ComPtr<ID3D12Resource2> CreateUploadBuffer(UINT64 size, ID3D12Device5* device)
{
    D3D12_HEAP_PROPERTIES heapProperties{
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

    ComPtr<ID3D12Resource2> resource;

    DXThrowIfFailed(device->CreateCommittedResource(
        &heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
        IID_PPV_ARGS(&resource)
    ));

    return resource;
}

ComPtr<ID3D12Resource2> CreateGPUBuffer(UINT64 size, D3D12_RESOURCE_STATES resourceState, ID3D12Device5* device)
{
    D3D12_HEAP_PROPERTIES heapProperties{
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

    ComPtr<ID3D12Resource2> resource;

    DXThrowIfFailed(device->CreateCommittedResource(
        &heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, resourceState, nullptr, IID_PPV_ARGS(&resource)
    ));

    return resource;
}

ComPtr<ID3D12Resource2> CreateDepthStencilBuffer(
    UINT width, UINT height, DXGI_FORMAT format, D3D12_RESOURCE_STATES resourceState, ID3D12Device5* device
)
{
    D3D12_HEAP_PROPERTIES heapProperties{
        .Type = D3D12_HEAP_TYPE_DEFAULT,
        .CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
        .MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
        .CreationNodeMask = 0,
        .VisibleNodeMask = 0
    };

    D3D12_RESOURCE_DESC resourceDesc{
        .Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
        .Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT,
        .Width = width,
        .Height = height,
        .DepthOrArraySize = 1,
        .MipLevels = 1,
        .Format = format,
        .SampleDesc{.Count = 1, .Quality = 0},
        .Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
        .Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
    };

    D3D12_CLEAR_VALUE clearValue{.Format = format, .DepthStencil{.Depth = 1.0f, .Stencil = 0}};

    ComPtr<ID3D12Resource2> resource;

    DXThrowIfFailed(device->CreateCommittedResource(
        &heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, resourceState, &clearValue, IID_PPV_ARGS(&resource)
    ));

    return resource;
}

void CopyDataToResource(ID3D12Resource2* resource, const void* data, UINT64 size, UINT64 offset)
{
    void* mappedData;
    DXThrowIfFailed(resource->Map(0, nullptr, &mappedData));
    std::memcpy(static_cast<BYTE*>(mappedData) + offset, data, size);
    resource->Unmap(0, nullptr);
}

void QueueTransition(
    ID3D12Resource2* resource, D3D12_RESOURCE_STATES oldState, D3D12_RESOURCE_STATES newState,
    std::vector<D3D12_RESOURCE_BARRIER>& barriers
)
{
    D3D12_RESOURCE_BARRIER barrier{
        .Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
        .Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
        .Transition{
            .pResource = resource,
            .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
            .StateBefore = oldState,
            .StateAfter = newState
        }
    };

    barriers.push_back(barrier);
}
