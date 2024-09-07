#pragma once

#include "auxiliary/win_include.h"

namespace Baltic
{
    class VertexBuffer
    {
    public:
        VertexBuffer(UINT64 size, ID3D12Device5* device);
        ~VertexBuffer() = default;

        void Bind(const ComPtr<ID3D12GraphicsCommandList6>& commandList) const;

    private:
        ComPtr<ID3D12Resource2> m_vertexBuffer;
    };

} // namespace Baltic
