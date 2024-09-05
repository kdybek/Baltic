#pragma once

#include "auxiliary/shader.h"
#include "auxiliary/types.h"
#include "d3d/dx_context.h"

namespace Baltic
{
    class PipelineState
    {
    public:
        PipelineState(const DXContext& dxContext);
        ~PipelineState() = default;

        PipelineState(const PipelineState&) = delete;
        PipelineState& operator=(const PipelineState&) = delete;

        void StageRootSignature(const DXRootSignatureComPtr& rootSignature);
        void StageVertexShader(const Shader& vertexShader);
        void StagePixelShader(const Shader& pixelShader);
        void StageInputLayout(const D3D12_INPUT_LAYOUT_DESC& inputLayout);

        void Finalize();

        [[nodiscard]] inline ID3D12PipelineState* Get() const { return m_pipelineState.Get(); }

    private:
        DXPipelineStateComPtr m_pipelineState;
        D3D12_GRAPHICS_PIPELINE_STATE_DESC m_desc;

        const DXContext& m_dxContext;
    };

}  // namespace Baltic
