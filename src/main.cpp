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

int main()
{
    int ret = 0;

    try {
        DXThrowIfFailed(RoInitialize(RO_INIT_SINGLETHREADED));

        DXDebugLayer dxDebugLayer;

        {
            DXContext dxContext;

            UploadBuffer cameraCBuffer(((sizeof(CameraCBuffer) + 255) & ~255), dxContext.GetDeviceComPtr().Get());
            UploadBuffer lightCBuffer(((sizeof(LightCBuffer) + 255) & ~255), dxContext.GetDeviceComPtr().Get());
            UploadBuffer uploadBuffer(1024, dxContext.GetDeviceComPtr().Get());
            GPUBuffer vertexBuffer(1024, dxContext.GetDeviceComPtr().Get());
            GPUBuffer indexBuffer(1024, dxContext.GetDeviceComPtr().Get());

            VertexBufferElement vertices[4]{
                {.position{-.5f, -.5f, 1.f}},
                {.position{-.5f, .5f, 1.f}},
                {.position{.5f, .5f, 1.f}},
                {.position{.5f, -.5f, 1.f}}
            };

            UINT32 indices[6]{0, 1, 3, 1, 2, 3};

            LightSource lightSource1{
                .position{-1.f, 1.f, 0.f},
                .color{0.4f, 1.f, 1.f},
                .intensity = .4f
            };
             
            LightSource lightSource2{
                .position{1.f, 1.f, 0.f},
                .color{1.f, 0.4f, 1.f},
                .intensity = .4f
            };

            LightCBuffer lightCBufferData{
                .lightSource{lightSource1, lightSource2},
                .lightCount = 2
            };
            
            lightCBuffer.CopyData(&lightCBufferData, sizeof(LightCBuffer));

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
            FLOAT xPlaneAngle = 0.f;

            while (!stop) {
                mainWindow.Update();

                Event event;
                while ((event = mainWindow.PollEvent()).type != EventType::None) {
                    if (event.type == EventType::Close) {
                        stop = TRUE;
                    }
                    else if (event.type == EventType::Resize) {
                        dxContext.Flush(FRAME_COUNT);
                        mainWindow.ResizeSwapChain(dxContext.GetDeviceComPtr().Get());
                        mainWindow.ConfineCursor();
                        mainWindow.CenterCursor();
                        lastCursorPos = mainWindow.GetCursorPosition();
                    }
                    else if (event.type == EventType::Focus) {
                        mainWindow.ConfineCursor();
                        mainWindow.CenterCursor();
                        lastCursorPos = mainWindow.GetCursorPosition();
                    }
                    else if (event.type == EventType::Move) {
                        mainWindow.ConfineCursor();
                        mainWindow.CenterCursor();
                        lastCursorPos = mainWindow.GetCursorPosition();
                    }
                    else if (event.type == EventType::KeyDown) {
                        if (event.key == Key::F11) {
                            mainWindow.SetFullscreen(!mainWindow.isFullscreen());
                            mainWindow.ConfineCursor();
                            mainWindow.CenterCursor();
                            lastCursorPos = mainWindow.GetCursorPosition();
                        }
                        keyStates[event.key] = TRUE;
                    }
                    else if (event.type == EventType::KeyUp) {
                        keyStates[event.key] = FALSE;
                    }
                    else if (event.type == EventType::MouseMove) {
                        FLOAT xAngle = static_cast<FLOAT>(event.cursorPosition.y - lastCursorPos.y) * -.001f;
                        FLOAT yAngle = static_cast<FLOAT>(event.cursorPosition.x - lastCursorPos.x) * -.001f;
                        FLOAT zTranslation = 0.f;
                        FLOAT xTranslation = 0.f;
                        if (keyStates.at(Key::W)) {
                            zTranslation -= .1f;
                        }
                        if (keyStates.at(Key::A)) {
                            xTranslation += .1f;
                        }
                        if (keyStates.at(Key::S)) {
                            zTranslation += .1f;
                        }
                        if (keyStates.at(Key::D)) {
                            xTranslation -= .1f;
                        }
                        DirectX::XMMATRIX rotationMatrix1 = DirectX::XMMatrixRotationX(-xPlaneAngle);
                        DirectX::XMMATRIX translationMatrix =
                            DirectX::XMMatrixTranslation(xTranslation, 0.f, zTranslation);
                        DirectX::XMMATRIX rotationMatrix2 = DirectX::XMMatrixRotationY(yAngle);
                        DirectX::XMMATRIX rotationMatrix3 = DirectX::XMMatrixRotationX(xAngle + xPlaneAngle);
                        viewMatrix = DirectX::XMMatrixMultiply(viewMatrix, rotationMatrix1);
                        viewMatrix = DirectX::XMMatrixMultiply(viewMatrix, translationMatrix);
                        viewMatrix = DirectX::XMMatrixMultiply(viewMatrix, rotationMatrix2);
                        viewMatrix = DirectX::XMMatrixMultiply(viewMatrix, rotationMatrix3);
                        xPlaneAngle += xAngle;
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

                CameraCBuffer cameraCBufferData{
                    .viewMatrix = viewMatrix,
                    .projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(
                        DirectX::XMConvertToRadians(60.f),
                        static_cast<FLOAT>(mainWindow.GetWidth()) / static_cast<FLOAT>(mainWindow.GetHeight()), .1f,
                        100.f
                    )
                };

                cameraCBuffer.CopyData(&cameraCBufferData, sizeof(CameraCBuffer));

                cmdList->SetGraphicsRootConstantBufferView(0, cameraCBuffer.GetComPtr()->GetGPUVirtualAddress());
                
                cmdList->SetGraphicsRootConstantBufferView(1, lightCBuffer.GetComPtr()->GetGPUVirtualAddress());

                cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);

                mainWindow.StageCmdEndFrame(cmdList.Get());

                dxContext.ExecuteCmdList();

                mainWindow.Present();

                std::this_thread::sleep_for(std::chrono::milliseconds(30));
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
