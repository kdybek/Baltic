#include "d3d/buffers.h"

#include "auxiliary/baltic_except.h"

namespace Baltic
{
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
                &heapPropertiesUpload, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                IID_PPV_ARGS(&m_vertexBuffer)
            ))) {
            throw BalticException("CreateCommittedResource");
        }
    }

    void VertexBuffer::Bind(const ComPtr<ID3D12GraphicsCommandList6>& commandList) const
    {
        D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {
            .BufferLocation = m_vertexBuffer->GetGPUVirtualAddress(),
            .SizeInBytes = static_cast<UINT>(m_vertexBuffer->GetDesc().Width),
            .StrideInBytes = sizeof(VertexBufferElement)
        };

        commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
    }

} // namespace Baltic
