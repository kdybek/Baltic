#pragma once

#include <vector>

namespace Baltic
{
    using Microsoft::WRL::ComPtr;

    struct VertexBufferElement
    {
        DirectX::XMFLOAT2 position;

    } __attribute__((packed));

    struct Mesh
    {
        std::vector<VertexBufferElement> vertices;
        std::vector<UINT32> indices;
    };

} // namespace Baltic
