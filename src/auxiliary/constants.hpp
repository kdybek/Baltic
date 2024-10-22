#pragma once

// clang-format off
#include "auxiliary/pch.hpp"
// clang-format on

inline constexpr DWORD EVENT_TIMEOUT = 20000;
inline constexpr UINT FRAME_COUNT = 2;

inline constexpr D3D12_INPUT_ELEMENT_DESC VB_INPUT_ELEMENT_DESC[]{
    {.SemanticName = "POSITION",
     .SemanticIndex = 0,
     .Format = DXGI_FORMAT_R32G32B32_FLOAT,
     .InputSlot = 0,
     .AlignedByteOffset = 0,
     .InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
     .InstanceDataStepRate = 0}
};
inline constexpr D3D12_INPUT_LAYOUT_DESC VB_INPUT_LAYOUT_DESC{
    .pInputElementDescs = VB_INPUT_ELEMENT_DESC, .NumElements = _countof(VB_INPUT_ELEMENT_DESC)
};

inline constexpr DXGI_FORMAT RTV_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;
inline constexpr DXGI_FORMAT DSV_FORMAT = DXGI_FORMAT_D32_FLOAT;

inline constexpr D3D12_GRAPHICS_PIPELINE_STATE_DESC DEFAULT_PIPELINE_STATE_DESC{
    .pRootSignature = nullptr,
    .VS{.pShaderBytecode = nullptr, .BytecodeLength = 0},
    .PS{.pShaderBytecode = nullptr, .BytecodeLength = 0},
    .DS{.pShaderBytecode = nullptr, .BytecodeLength = 0},
    .HS{.pShaderBytecode = nullptr, .BytecodeLength = 0},
    .GS{.pShaderBytecode = nullptr, .BytecodeLength = 0},
    .StreamOutput{
        .pSODeclaration = nullptr, .NumEntries = 0, .pBufferStrides = nullptr, .NumStrides = 0, .RasterizedStream = 0
    },
    .BlendState{
        .AlphaToCoverageEnable = FALSE,
        .IndependentBlendEnable = FALSE,
        .RenderTarget{
            {.BlendEnable = FALSE,
             .LogicOpEnable = FALSE,
             .SrcBlend = D3D12_BLEND_ONE,
             .DestBlend = D3D12_BLEND_ZERO,
             .BlendOp = D3D12_BLEND_OP_ADD,
             .SrcBlendAlpha = D3D12_BLEND_ONE,
             .DestBlendAlpha = D3D12_BLEND_ZERO,
             .BlendOpAlpha = D3D12_BLEND_OP_ADD,
             .LogicOp = D3D12_LOGIC_OP_NOOP,
             .RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL}
        }
    },
    .SampleMask = UINT_MAX,
    .RasterizerState{
        .FillMode = D3D12_FILL_MODE_SOLID,
        .CullMode = D3D12_CULL_MODE_BACK,
        .FrontCounterClockwise = FALSE,
        .DepthBias = D3D12_DEFAULT_DEPTH_BIAS,
        .DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
        .SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
        .DepthClipEnable = FALSE,
        .MultisampleEnable = FALSE,
        .AntialiasedLineEnable = FALSE,
        .ForcedSampleCount = 0,
        .ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF
    },
    .DepthStencilState{
        .DepthEnable = FALSE,
        .DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL,
        .DepthFunc = D3D12_COMPARISON_FUNC_LESS,
        .StencilEnable = FALSE,
        .StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK,
        .StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK,
        .FrontFace{
            .StencilFailOp = D3D12_STENCIL_OP_KEEP,
            .StencilDepthFailOp = D3D12_STENCIL_OP_KEEP,
            .StencilPassOp = D3D12_STENCIL_OP_KEEP,
            .StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS
        },
        .BackFace{
            .StencilFailOp = D3D12_STENCIL_OP_KEEP,
            .StencilDepthFailOp = D3D12_STENCIL_OP_KEEP,
            .StencilPassOp = D3D12_STENCIL_OP_KEEP,
            .StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS
        }
    },
    .InputLayout{.pInputElementDescs = nullptr, .NumElements = 0},
    .IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED,
    .PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
    .NumRenderTargets = 1,
    .RTVFormats{RTV_FORMAT},
    .DSVFormat = DSV_FORMAT,
    .SampleDesc{.Count = 1, .Quality = 0},
    .NodeMask = 0,
    .CachedPSO{.pCachedBlob = nullptr, .CachedBlobSizeInBytes = 0},
    .Flags = D3D12_PIPELINE_STATE_FLAG_NONE
};

inline constexpr std::array<UINT, 8> HANDLED_MESSAGES{WM_CLOSE, WM_SIZE,    WM_SETFOCUS, WM_KILLFOCUS,
                                                      WM_MOVE,  WM_KEYDOWN, WM_KEYUP,    WM_MOUSEMOVE};
