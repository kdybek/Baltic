#pragma once

#include "auxiliary/win_include.h"

namespace Baltic
{
    class VertexBuffer
    {
    public:
        VertexBuffer(UINT64 size, ID3D12Device5* device);
        ~VertexBuffer() = default;

        VertexBuffer(const VertexBuffer&) = delete;
        VertexBuffer& operator=(const VertexBuffer&) = delete;

        void StageCmdBind(UINT size, UINT stride, ID3D12GraphicsCommandList6* commandList) const;

        [[nodiscard]] inline const ComPtr<ID3D12Resource2>& GetComPtr() { return m_vertexBuffer; }

    private:
        ComPtr<ID3D12Resource2> m_vertexBuffer;
        D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
    };

} // namespace Baltic
