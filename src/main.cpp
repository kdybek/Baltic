#include <iostream>

#include "auxiliary/baltic_except.h"
#include "auxiliary/constants.h"
#include "auxiliary/input_layouts.h"
#include "auxiliary/win_include.h"
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
        if (FAILED(RoInitialize(RO_INIT_SINGLETHREADED))) {
            throw BalticException("RoInitialize");
        }

        DXDebugLayer dxDebugLayer;

        {
            VertexBufferElement vertices[3] = {
                {.position = {0.0f, 0.5f}}, {.position = {0.5f, -0.5f}}, {.position = {-0.5f, -0.5f}}
            };

            DXContext dxContext;

            UploadBuffer uploadBuffer(1024, dxContext.GetDeviceComPtr().Get());
            VertexBuffer vertexBuffer(1024, dxContext.GetDeviceComPtr().Get());

            dxContext.ResetCmdList();

            uploadBuffer.CopyData(vertices, sizeof(vertices));
            uploadBuffer.StageCmdUpload(
                vertexBuffer.GetComPtr().Get(), sizeof(vertices), dxContext.GetCmdListComPtr().Get()
            );
            vertexBuffer.SetView(sizeof(vertices), sizeof(VertexBufferElement));
            dxContext.ExecuteCmdList();

            Shader vertexShader("vertex_shader.cso");
            Shader pixelShader("pixel_shader.cso");
            Shader rootSignatureShader("root_signature.cso");

            ComPtr<ID3D12RootSignature> rootSignature;
            dxContext.GetDeviceComPtr()->CreateRootSignature(
                0, rootSignatureShader.GetData(), rootSignatureShader.GetSize(), IID_PPV_ARGS(&rootSignature)
            );

            PipelineState pipelineState;

            pipelineState.StageRootSignature(rootSignature.Get());
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
                cmdList->SetGraphicsRootSignature(rootSignature.Get());

                vertexBuffer.StageCmdBind(cmdList.Get());
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

                cmdList->DrawInstanced(_countof(vertices), 1, 0, 0);

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
