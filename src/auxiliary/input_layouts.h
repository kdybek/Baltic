#pragma once

// clang-format off
#include "auxiliary/win_include.h"
// clang-format on

#include <directxmath.h>

namespace Baltic
{
    // Input Element Descriptors
    inline D3D12_INPUT_ELEMENT_DESC g_vertexBufferLayoutElements[]{
        {.SemanticName = "Position",
         .SemanticIndex = 0,
         .Format = DXGI_FORMAT_R32G32_FLOAT,
         .InputSlot = 0,
         .AlignedByteOffset = 0,
         .InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
         .InstanceDataStepRate = 0}
    };

    // Input Layout Descriptors
    inline D3D12_INPUT_LAYOUT_DESC g_vertexBufferLayout{
        .pInputElementDescs = g_vertexBufferLayoutElements, .NumElements = _countof(g_vertexBufferLayoutElements)
    };

    // Input Element Types
    struct VertexBufferElement
    {
        DirectX::XMFLOAT2 position;

    } __attribute__((packed));

} // namespace Baltic
