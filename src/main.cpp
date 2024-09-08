// clang-format off
#include "auxiliary/pch.h"
// clang-format on

#include <iostream>

#include "auxiliary/baltic_exception.h"
#include "auxiliary/constants.h"
#include "auxiliary/input_layouts.h"
#include "d3d/buffers.h"
#include "d3d/dx_context.h"
#include "d3d/dx_window.h"
#include "d3d/pipeline_state.h"
#include "d3d/shader.h"
#include "debug/dx_debug_layer.h"

using namespace Baltic;

int main()
{
    int ret = 0;

    try {
        DXThrowIfFailed(RoInitialize(RO_INIT_SINGLETHREADED));

        DXDebugLayer dxDebugLayer;

        {
            DXContext dxContext;

            VertexBufferElement vertices[4]{
                {.position{-0.5f, -0.5f}}, {.position{-0.5f, 0.5f}}, {.position{0.5f, 0.5f}}, {.position{0.5f, -0.5f}}
            };

            UINT32 indices[6]{0, 1, 3, 1, 2, 3};

            UploadBuffer uploadBuffer(1024, dxContext.GetDeviceComPtr().Get());
            GPUBuffer vertexBuffer(1024, dxContext.GetDeviceComPtr().Get());
            GPUBuffer indexBuffer(1024, dxContext.GetDeviceComPtr().Get());

            dxContext.ResetCmdList();
            uploadBuffer.CopyData(vertices, sizeof(vertices));
            uploadBuffer.StageCmdUpload(
                vertexBuffer.GetComPtr().Get(), sizeof(vertices), dxContext.GetCmdListComPtr().Get()
            );
            dxContext.ExecuteCmdList();
            dxContext.ResetCmdList();
            uploadBuffer.CopyData(indices, sizeof(indices));
            uploadBuffer.StageCmdUpload(
                indexBuffer.GetComPtr().Get(), sizeof(indices), dxContext.GetCmdListComPtr().Get()
            );
            dxContext.ExecuteCmdList();

            D3D12_VERTEX_BUFFER_VIEW vertexBufferView{
                .BufferLocation = vertexBuffer.GetComPtr()->GetGPUVirtualAddress(),
                .SizeInBytes = sizeof(vertices),
                .StrideInBytes = sizeof(VertexBufferElement)
            };

            D3D12_INDEX_BUFFER_VIEW indexBufferView{
                .BufferLocation = indexBuffer.GetComPtr()->GetGPUVirtualAddress(),
                .SizeInBytes = sizeof(indices),
                .Format = DXGI_FORMAT_R32_UINT
            };

            Shader vertexShader("vertex_shader.cso");
            Shader pixelShader("pixel_shader.cso");
            RootSignature rootSignature("root_signature.cso", dxContext.GetDeviceComPtr().Get());

            PipelineState pipelineState;

            pipelineState.StageRootSignature(rootSignature.GetComPtr().Get());
            pipelineState.StageVertexShader(vertexShader);
            pipelineState.StagePixelShader(pixelShader);
            pipelineState.StageInputLayout(g_vertexBufferLayout);
            pipelineState.Finalize(dxContext.GetDeviceComPtr().Get());

            DXWindow mainWindow(1920, 1080, dxContext);
            mainWindow.SetFullscreen(TRUE);

            while (!mainWindow.ShouldClose()) {
                mainWindow.Update();

                if (mainWindow.ShouldResize()) {
                    dxContext.Flush(FRAME_COUNT);
                    mainWindow.ResizeSwapChain(dxContext.GetDeviceComPtr().Get());
                }

                dxContext.ResetCmdList();

                const auto& cmdList = dxContext.GetCmdListComPtr();

                mainWindow.StageCmdBeginFrame(cmdList.Get());

                cmdList->SetPipelineState(pipelineState.GetComPtr().Get());
                cmdList->SetGraphicsRootSignature(rootSignature.GetComPtr().Get());

                cmdList->IASetVertexBuffers(0, 1, &vertexBufferView);
                cmdList->IASetIndexBuffer(&indexBufferView);
                cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

                D3D12_VIEWPORT viewport{
                    .TopLeftX = 0.f,
                    .TopLeftY = 0.f,
                    .Width = static_cast<FLOAT>(mainWindow.GetWidth()),
                    .Height = static_cast<FLOAT>(mainWindow.GetHeight()),
                    .MinDepth = 0.f,
                    .MaxDepth = 1.f
                };

                cmdList->RSSetViewports(1, &viewport);

                D3D12_RECT scissorRect{
                    .left = 0,
                    .top = 0,
                    .right = static_cast<LONG>(mainWindow.GetWidth()),
                    .bottom = static_cast<LONG>(mainWindow.GetHeight())
                };

                cmdList->RSSetScissorRects(1, &scissorRect);

                cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);

                mainWindow.StageCmdEndFrame(cmdList.Get());

                dxContext.ExecuteCmdList();

                mainWindow.Present();
            }

            dxContext.Flush(FRAME_COUNT);
        }

        dxDebugLayer.ReportLiveObjects();
    }
    catch (const BalticException& e) {
        std::cout << e.what() << '\n';
        ret = 1;
    }

    RoUninitialize();

    return ret;
}
