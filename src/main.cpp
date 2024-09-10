// clang-format off
#include "auxiliary/pch.h"
// clang-format on

#include <chrono>
#include <iostream>
#include <thread>
#include <unordered_map>

#include "auxiliary/baltic_exception.h"
#include "auxiliary/constants.h"
#include "auxiliary/types.h"
#include "d3d/buffers.h"
#include "d3d/dx_context.h"
#include "d3d/dx_window.h"
#include "d3d/pipeline_state.h"
#include "d3d/shader.h"
#include "debug/dx_debug_layer.h"

using namespace Baltic;

DirectX::XMMATRIX UpdateViewMatrix(const std::unordered_map<Key, BOOL>& keyStates, DirectX::XMMATRIX viewMatrix)
{
    DirectX::XMVECTOR forward = viewMatrix.r[2];
    DirectX::XMVECTOR right = viewMatrix.r[0];

    if (keyStates.at(Key::W)) {
        viewMatrix.r[3] = DirectX::XMVectorSubtract(viewMatrix.r[3], forward);
    }
    if (keyStates.at(Key::A)) {
        viewMatrix.r[3] = DirectX::XMVectorAdd(viewMatrix.r[3], right);
    }
    if (keyStates.at(Key::S)) {
        viewMatrix.r[3] = DirectX::XMVectorAdd(viewMatrix.r[3], forward);
    }
    if (keyStates.at(Key::D)) {
        viewMatrix.r[3] = DirectX::XMVectorSubtract(viewMatrix.r[3], right);
    }

    return viewMatrix;
}

int main()
{
    int ret = 0;

    try {
        DXThrowIfFailed(RoInitialize(RO_INIT_SINGLETHREADED));

        DXDebugLayer dxDebugLayer;

        {
            DXContext dxContext;

            VertexBufferElement vertices[4]{
                {.position{-.5f, -.5f, 1.f}},
                {.position{-.5f, .5f, 1.f}},
                {.position{.5f, .5f, 1.f}},
                {.position{.5f, -.5f, 1.f}}
            };

            UINT32 indices[6]{0, 1, 3, 1, 2, 3};

            UploadBuffer cameraCBuffer(sizeof(CameraCBuffer), dxContext.GetDeviceComPtr().Get());
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
            Shader geometryShader("geometry_shader.cso");
            Shader pixelShader("pixel_shader.cso");
            RootSignature rootSignature("root_signature.cso", dxContext.GetDeviceComPtr().Get());

            PipelineState pipelineState;

            pipelineState.StageRootSignature(rootSignature.GetComPtr().Get());
            pipelineState.StageVertexShader(vertexShader);
            pipelineState.StageGeometryShader(geometryShader);
            pipelineState.StagePixelShader(pixelShader);
            pipelineState.StageInputLayout(VB_INPUT_LAYOUT_DESC);
            pipelineState.Finalize(dxContext.GetDeviceComPtr().Get());

            DXWindow mainWindow(1920, 1080, dxContext);
            mainWindow.SetFullscreen(TRUE);

            DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixIdentity();

            std::unordered_map<Key, BOOL> keyStates{
                {Key::W, FALSE}, {Key::A, FALSE}, {Key::S, FALSE}, {Key::D, FALSE}, {Key::F11, FALSE}
            };

            BOOL stop = FALSE;
            POINT lastCursorPos = mainWindow.GetCursorPosition();

            while (!stop) {
                mainWindow.Update();

                WindowEvent event;
                while ((event = mainWindow.PollEvent()).type != WindowEventType::None) {
                    if (event.type == WindowEventType::Close) {
                        stop = TRUE;
                    }
                    else if (event.type == WindowEventType::Resize) {
                        dxContext.Flush(FRAME_COUNT);
                        mainWindow.ResizeSwapChain(dxContext.GetDeviceComPtr().Get());
                        mainWindow.ConfineCursor();
                        mainWindow.CenterCursor();
                        lastCursorPos = mainWindow.GetCursorPosition();
                    }
                    else if (event.type == WindowEventType::Focus) {
                        mainWindow.ConfineCursor();
                        mainWindow.CenterCursor();
                        lastCursorPos = mainWindow.GetCursorPosition();
                    }
                    else if (event.type == WindowEventType::Move) {
                        mainWindow.ConfineCursor();
                        mainWindow.CenterCursor();
                        lastCursorPos = mainWindow.GetCursorPosition();
                    }
                    else if (event.type == WindowEventType::KeyDown) {
                        if (event.key == Key::F11) {
                            mainWindow.SetFullscreen(!mainWindow.isFullscreen());
                            mainWindow.ConfineCursor();
                            mainWindow.CenterCursor();
                            lastCursorPos = mainWindow.GetCursorPosition();
                        }
                        keyStates[event.key] = TRUE;
                    }
                    else if (event.type == WindowEventType::KeyUp) {
                        keyStates[event.key] = FALSE;
                    }
                    else if (event.type == WindowEventType::MouseMove) {
                        DirectX::XMVECTOR cursorMovement = DirectX::XMVectorSet(
                            static_cast<FLOAT>(event.cursorPosition.x - lastCursorPos.x),
                            static_cast<FLOAT>(event.cursorPosition.y - lastCursorPos.y), 0.f, 0.f
                        );
                        mainWindow.CenterCursor();
                        lastCursorPos = mainWindow.GetCursorPosition();
                    }
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

                viewMatrix = UpdateViewMatrix(keyStates, viewMatrix);

                CameraCBuffer cameraCBufferData{
                    .viewMatrix = viewMatrix,
                    .projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(
                        DirectX::XMConvertToRadians(90.f),
                        static_cast<FLOAT>(mainWindow.GetWidth()) / static_cast<FLOAT>(mainWindow.GetHeight()), .1f,
                        100.f
                    )
                };

                cameraCBuffer.CopyData(&cameraCBufferData, sizeof(CameraCBuffer));

                cmdList->SetGraphicsRootConstantBufferView(0, cameraCBuffer.GetComPtr()->GetGPUVirtualAddress());

                cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);

                mainWindow.StageCmdEndFrame(cmdList.Get());

                dxContext.ExecuteCmdList();

                mainWindow.Present();

                std::this_thread::sleep_for(std::chrono::milliseconds(160));
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
