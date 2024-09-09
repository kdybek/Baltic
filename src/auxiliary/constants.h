#pragma once

// clang-format off
#include "auxiliary/pch.h"
// clang-format on

namespace Baltic
{
    inline constexpr DWORD EVENT_TIMEOUT = 20000;
    inline constexpr UINT FRAME_COUNT = 2;

    inline constexpr D3D12_INPUT_ELEMENT_DESC VB_INPUT_ELEMENT_DESC[]{
        {.SemanticName = "Position",
         .SemanticIndex = 0,
         .Format = DXGI_FORMAT_R32G32_FLOAT,
         .InputSlot = 0,
         .AlignedByteOffset = 0,
         .InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
         .InstanceDataStepRate = 0}
    };

    inline constexpr D3D12_INPUT_LAYOUT_DESC VB_INPUT_LAYOUT_DESC{
        .pInputElementDescs = VB_INPUT_ELEMENT_DESC, .NumElements = _countof(VB_INPUT_ELEMENT_DESC)
    };
} // namespace Baltic
