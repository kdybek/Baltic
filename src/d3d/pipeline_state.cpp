#include "d3d/pipeline_state.h"

namespace Baltic
{
    namespace
    {
        constexpr D3D12_GRAPHICS_PIPELINE_STATE_DESC g_defaultPipelineStateDesc = {
            .pRootSignature = nullptr,
            .VS{.pShaderBytecode = nullptr, .BytecodeLength = 0},
            .PS{.pShaderBytecode = nullptr, .BytecodeLength = 0},
            .DS{.pShaderBytecode = nullptr, .BytecodeLength = 0},
            .HS{.pShaderBytecode = nullptr, .BytecodeLength = 0},
            .GS{.pShaderBytecode = nullptr, .BytecodeLength = 0},
            .StreamOutput{
                .pSODeclaration = nullptr,
                .NumEntries = 0,
                .pBufferStrides = nullptr,
                .NumStrides = 0,
                .RasterizedStream = 0
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
                .CullMode = D3D12_CULL_MODE_NONE,
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
                .DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO,
                .DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS,
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
            .RTVFormats{DXGI_FORMAT_R8G8B8A8_UNORM},
            .DSVFormat = DXGI_FORMAT_UNKNOWN,
            .SampleDesc{.Count = 1, .Quality = 0},
            .NodeMask = 0,
            .CachedPSO{.pCachedBlob = nullptr, .CachedBlobSizeInBytes = 0},
            .Flags = D3D12_PIPELINE_STATE_FLAG_NONE
        };

    } // namespace

    PipelineState::PipelineState() : m_desc(g_defaultPipelineStateDesc) {}

    void PipelineState::StageRootSignature(const ComPtr<ID3D12RootSignature>& rootSignature)
    {
        m_desc.pRootSignature = rootSignature.Get();
    }

    void PipelineState::StageVertexShader(const Shader& vertexShader)
    {
        m_desc.VS = {vertexShader.GetData(), vertexShader.GetSize()};
    }

    void PipelineState::StagePixelShader(const Shader& pixelShader)
    {
        m_desc.PS = {pixelShader.GetData(), pixelShader.GetSize()};
    }

    void PipelineState::StageInputLayout(const D3D12_INPUT_LAYOUT_DESC& inputLayout)
    {
        m_desc.InputLayout = inputLayout;
    }

    void PipelineState::Finalize(ID3D12Device* device)
    {
        device->CreateGraphicsPipelineState(&m_desc, IID_PPV_ARGS(&m_pipelineState));
    }

    void PipelineState::Reset()
    {
        m_pipelineState.Reset();
        m_desc = g_defaultPipelineStateDesc;
    }

} // namespace Baltic
