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
#include "d3d/dx_context.h"
#include "d3d/dx_resource.h"
#include "d3d/dx_window.h"
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

            std::vector<D3D12_RESOURCE_BARRIER> barriers;

            ComPtr<ID3D12Resource2> uploadBuffer = CreateUploadBuffer(1024, dxContext.GetDeviceComPtr().Get());
            ComPtr<ID3D12Resource2> vertexBuffer =
                CreateGPUBuffer(1024, D3D12_RESOURCE_STATE_COMMON, dxContext.GetDeviceComPtr().Get());
            ComPtr<ID3D12Resource2> indexBuffer =
                CreateGPUBuffer(1024, D3D12_RESOURCE_STATE_COMMON, dxContext.GetDeviceComPtr().Get());
            ComPtr<ID3D12Resource2> cameraCBuffer =
                CreateUploadBuffer(((sizeof(CameraCBuffer) + 255) & ~255), dxContext.GetDeviceComPtr().Get());
            ComPtr<ID3D12Resource2> lightCBuffer =
                CreateUploadBuffer(((sizeof(LightCBuffer) + 255) & ~255), dxContext.GetDeviceComPtr().Get());

            VertexBufferElement vertices[12]{{.position{-.5f, -.5f, 1.f}}, {.position{-.5f, .5f, 1.f}},
                                             {.position{.5f, .5f, 1.f}},   {.position{.5f, -.5f, 1.f}},
                                             {.position{-.5f, -.5f, 2.f}}, {.position{-.5f, .5f, 2.f}},
                                             {.position{.5f, .5f, 2.f}},   {.position{.5f, -.5f, 2.f}},
                                             {.position{-.5f, -.5f, 3.f}}, {.position{-.5f, .5f, 3.f}},
                                             {.position{.5f, .5f, 3.f}},   {.position{.5f, -.5f, 3.f}}};

            UINT32 indices[39]{0, 1, 3, 1, 2, 3, 4, 5, 0, 5, 1, 0, 7, 6, 4, 6, 5, 4, 3, 2,
                               7, 2, 6, 7, 1, 5, 2, 5, 6, 2, 4, 0, 7, 0, 3, 7, 8, 9, 10};

            LightSource lightSource1{.position{-1.f, 1.f, 0.f}, .color{0.4f, 1.f, 1.f}, .intensity = .9f};

            LightSource lightSource2{.position{1.f, -1.f, 3.f}, .color{1.f, 0.4f, 1.f}, .intensity = .9f};

            LightCBuffer lightCBufferData{.lightSource{lightSource1, lightSource2}, .lightCount = 2};

            CopyDataToResource(lightCBuffer.Get(), &lightCBufferData, sizeof(LightCBuffer));

            dxContext.ResetCmdList();
            CopyDataToResource(uploadBuffer.Get(), vertices, sizeof(vertices));
            QueueTransition(vertexBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST, barriers);
            dxContext.GetCmdListComPtr()->ResourceBarrier(barriers.size(), barriers.data());
            barriers.clear();
            dxContext.GetCmdListComPtr()->CopyBufferRegion(
                vertexBuffer.Get(), 0, uploadBuffer.Get(), 0, sizeof(vertices)
            );
            dxContext.ExecuteCmdList();

            dxContext.ResetCmdList();
            CopyDataToResource(uploadBuffer.Get(), indices, sizeof(indices));
            QueueTransition(indexBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST, barriers);
            dxContext.GetCmdListComPtr()->ResourceBarrier(barriers.size(), barriers.data());
            barriers.clear();
            dxContext.GetCmdListComPtr()->CopyBufferRegion(
                indexBuffer.Get(), 0, uploadBuffer.Get(), 0, sizeof(indices)
            );
            dxContext.ExecuteCmdList();

            dxContext.ResetCmdList();
            QueueTransition(
                vertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
                barriers
            );
            QueueTransition(
                indexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER, barriers
            );
            dxContext.GetCmdListComPtr()->ResourceBarrier(barriers.size(), barriers.data());
            barriers.clear();
            dxContext.ExecuteCmdList();

            D3D12_VERTEX_BUFFER_VIEW vertexBufferView{
                .BufferLocation = vertexBuffer->GetGPUVirtualAddress(),
                .SizeInBytes = sizeof(vertices),
                .StrideInBytes = sizeof(VertexBufferElement)
            };

            D3D12_INDEX_BUFFER_VIEW indexBufferView{
                .BufferLocation = indexBuffer->GetGPUVirtualAddress(),
                .SizeInBytes = sizeof(indices),
                .Format = DXGI_FORMAT_R32_UINT
            };

            Shader vertexShader("vertex_shader.cso");
            Shader geometryShader("geometry_shader.cso");
            Shader pixelShader("pixel_shader.cso");
            RootSignature rootSignature("root_signature.cso", dxContext.GetDeviceComPtr().Get());

            D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {
                .DepthEnable = TRUE,
                .DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL,
                .DepthFunc = D3D12_COMPARISON_FUNC_LESS,
                .StencilEnable = FALSE,
                .StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK,
                .StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK,
                .FrontFace =
                    {.StencilFailOp = D3D12_STENCIL_OP_KEEP,
                     .StencilDepthFailOp = D3D12_STENCIL_OP_KEEP,
                     .StencilPassOp = D3D12_STENCIL_OP_KEEP,
                     .StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS},
                .BackFace =
                    {.StencilFailOp = D3D12_STENCIL_OP_KEEP,
                     .StencilDepthFailOp = D3D12_STENCIL_OP_KEEP,
                     .StencilPassOp = D3D12_STENCIL_OP_KEEP,
                     .StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS}
            };

            D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc = DEFAULT_PIPELINE_STATE_DESC;
            pipelineStateDesc.pRootSignature = rootSignature.GetComPtr().Get();
            pipelineStateDesc.VS = {vertexShader.GetData(), vertexShader.GetSize()};
            pipelineStateDesc.GS = {geometryShader.GetData(), geometryShader.GetSize()};
            pipelineStateDesc.PS = {pixelShader.GetData(), pixelShader.GetSize()};
            pipelineStateDesc.DepthStencilState = depthStencilDesc;
            pipelineStateDesc.InputLayout = VB_INPUT_LAYOUT_DESC;
            ComPtr<ID3D12PipelineState> pipelineState;
            dxContext.GetDeviceComPtr()->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(&pipelineState));

            DXWindow mainWindow(1920, 1080, dxContext);
            mainWindow.SetFullscreen(TRUE);

            DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixIdentity();

            std::unordered_map<Key, BOOL> keyStates{{Key::W, FALSE},  {Key::A, FALSE},     {Key::S, FALSE},
                                                    {Key::D, FALSE},  {Key::Space, FALSE}, {Key::Shift, FALSE},
                                                    {Key::F11, FALSE}};

            BOOL end = FALSE;
            POINT lastCursorPos = mainWindow.GetCursorPosition();
            FLOAT xzPlaneAngle = 0.f;

            while (!end) {
                mainWindow.Update();

                Event event;
                while ((event = mainWindow.PollEvent()).type != EventType::None) {
                    if (event.type == EventType::Close) {
                        end = TRUE;
                    }
                    else if (event.type == EventType::Resize) {
                        dxContext.Flush(FRAME_COUNT);
                        mainWindow.Resize(dxContext.GetDeviceComPtr().Get());
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
                        FLOAT xTranslation = 0.f;
                        FLOAT yTranslation = 0.f;
                        FLOAT zTranslation = 0.f;

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

                        if (keyStates.at(Key::Space)) {
                            yTranslation -= .1f;
                        }

                        if (keyStates.at(Key::Shift)) {
                            yTranslation += .1f;
                        }

                        if (xAngle + xzPlaneAngle > DirectX::XM_PIDIV2) {
                            xAngle = DirectX::XM_PIDIV2 - xzPlaneAngle;
                        }
                        else if (xAngle + xzPlaneAngle < -DirectX::XM_PIDIV2) {
                            xAngle = -DirectX::XM_PIDIV2 - xzPlaneAngle;
                        }

                        DirectX::XMMATRIX rotationMatrix1 = DirectX::XMMatrixRotationX(-xzPlaneAngle);
                        DirectX::XMMATRIX translationMatrix =
                            DirectX::XMMatrixTranslation(xTranslation, yTranslation, zTranslation);
                        DirectX::XMMATRIX rotationMatrix2 = DirectX::XMMatrixRotationY(yAngle);
                        DirectX::XMMATRIX rotationMatrix3 = DirectX::XMMatrixRotationX(xAngle + xzPlaneAngle);
                        viewMatrix = DirectX::XMMatrixMultiply(viewMatrix, rotationMatrix1);
                        viewMatrix = DirectX::XMMatrixMultiply(viewMatrix, translationMatrix);
                        viewMatrix = DirectX::XMMatrixMultiply(viewMatrix, rotationMatrix2);
                        viewMatrix = DirectX::XMMatrixMultiply(viewMatrix, rotationMatrix3);
                        xzPlaneAngle += xAngle;
                        mainWindow.CenterCursor();
                        lastCursorPos = mainWindow.GetCursorPosition();
                    }
                }

                dxContext.ResetCmdList();

                const auto& cmdList = dxContext.GetCmdListComPtr();

                mainWindow.QueuePreRenderingTransitions(barriers);
                cmdList->ResourceBarrier(barriers.size(), barriers.data());
                barriers.clear();
                FLOAT clearColor[]{.1f, .1f, .1f, 1.f};
                cmdList->ClearRenderTargetView(*mainWindow.GetBackBufferRTVHandlePtr(), clearColor, 0, nullptr);
                cmdList->ClearDepthStencilView(
                    *mainWindow.GetDSVHandlePtr(), D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, nullptr
                );
                cmdList->OMSetRenderTargets(
                    1, mainWindow.GetBackBufferRTVHandlePtr(), FALSE, mainWindow.GetDSVHandlePtr()
                );

                cmdList->SetPipelineState(pipelineState.Get());
                cmdList->SetGraphicsRootSignature(rootSignature.GetComPtr().Get());

                cmdList->IASetVertexBuffers(0, 1, &vertexBufferView);
                cmdList->IASetIndexBuffer(&indexBufferView);
                cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

                cmdList->RSSetViewports(1, mainWindow.GetViewportPtr());

                cmdList->RSSetScissorRects(1, mainWindow.GetScissorRectPtr());

                CameraCBuffer cameraCBufferData{
                    .viewMatrix = viewMatrix,
                    .projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(
                        DirectX::XMConvertToRadians(60.f),
                        static_cast<FLOAT>(mainWindow.GetWidth()) / static_cast<FLOAT>(mainWindow.GetHeight()), .1f,
                        100.f
                    )
                };

                CopyDataToResource(cameraCBuffer.Get(), &cameraCBufferData, sizeof(CameraCBuffer));

                cmdList->SetGraphicsRootConstantBufferView(0, cameraCBuffer->GetGPUVirtualAddress());

                cmdList->SetGraphicsRootConstantBufferView(1, lightCBuffer->GetGPUVirtualAddress());

                cmdList->DrawIndexedInstanced(39, 1, 0, 0, 0);

                mainWindow.QueuePostRenderingTransitions(barriers);
                cmdList->ResourceBarrier(barriers.size(), barriers.data());
                barriers.clear();

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
