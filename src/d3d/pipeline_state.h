#pragma once

// clang-format off
#include "auxiliary/win_include.h"
// clang-format on

#include "d3d/shader.h"

namespace Baltic
{
    class PipelineState
    {
    public:
        PipelineState();
        ~PipelineState() = default;

        PipelineState(const PipelineState&) = delete;
        PipelineState& operator=(const PipelineState&) = delete;

        void StageRootSignature(ID3D12RootSignature* rootSignature);
        void StageVertexShader(const Shader& vertexShader);
        void StagePixelShader(const Shader& pixelShader);
        void StageInputLayout(const D3D12_INPUT_LAYOUT_DESC& inputLayout);

        void Finalize(ID3D12Device* device);
        void Reset();

        [[nodiscard]] inline const ComPtr<ID3D12PipelineState>& GetComPtr() { return m_pipelineState; }

    private:
        ComPtr<ID3D12PipelineState> m_pipelineState;
        D3D12_GRAPHICS_PIPELINE_STATE_DESC m_desc;
    };

} // namespace Baltic
