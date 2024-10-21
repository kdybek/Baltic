// clang-format off
#include "auxiliary/pch.hpp"
// clang-format on

#include "auxiliary/baltic_exception.hpp"
#include "auxiliary/camera.hpp"
#include "auxiliary/constants.hpp"
#include "auxiliary/types.hpp"
#include "d3d/dx_context.hpp"
#include "d3d/dx_resource.hpp"
#include "d3d/dx_window.hpp"
#include "d3d/shader.hpp"
#include "debug/dx_debug_layer.hpp"
#include "imgui/gui.hpp"

// Forward declarations of auxiliary functions
SIZE_T AlignUp(SIZE_T size, SIZE_T alignment);
Mesh CreateXZPlane(FLOAT xSize, FLOAT zSize, UINT32 xSegments, UINT32 zSegments);
void ResetKeyStates(std::unordered_map<WPARAM, BOOL>& keyStates);
template <typename T>
SIZE_T VecDataSize(const std::vector<T>& vec);

// Entry point
INT WINAPI wWinMain(
    _In_ [[maybe_unused]] HINSTANCE instance, _In_opt_ [[maybe_unused]] HINSTANCE prev_instance,
    _In_ [[maybe_unused]] PWSTR cmd_line, _In_ [[maybe_unused]] INT cmd_show
)
{
    INT ret = 0;

    try {
        DXDebugLayer dxDebugLayer;

        {
            DXContext dxContext;

            std::vector<D3D12_RESOURCE_BARRIER> barriers;

            Model plane = {
                .mesh = CreateXZPlane(10.f, 10.f, 20, 20),
                .worldMatrix = DirectX::XMMatrixTranslation(0.f, -4.f, 0.f),
                .color{0.f, .3f, .8f}
            };

            LightSource lightSource1{.position{5.f, 10.f, 5.f}, .color{1.f, 1.f, 1.f}, .intensity = 80.f};
            LightSource lightSource2{.position{3.f, 2.f, 2.f}, .color{1.f, .4f, 0.f}, .intensity = 40.f};
            LightBuffer lightBufferData{
                .lightSources{lightSource1, lightSource2}, .numLights = 2, .viewDirection{0.f, 0.f, 1.f}
            };

            SIZE_T vertexBufferSize = AlignUp(VecDataSize(plane.mesh.vertices), 256);
            SIZE_T indexBufferSize = AlignUp(VecDataSize(plane.mesh.indices), 256);
            SIZE_T uploadBufferSize = vertexBufferSize + indexBufferSize;
            SIZE_T constantBufferSize = AlignUp(sizeof(ConstantBuffer), 256);
            SIZE_T lightBufferSize = AlignUp(sizeof(LightBuffer), 256);

            ComPtr<ID3D12Resource2> vertexBuffer =
                CreateGPUBuffer(vertexBufferSize, D3D12_RESOURCE_STATE_COMMON, dxContext.GetDeviceComPtr().Get());
            ComPtr<ID3D12Resource2> indexBuffer =
                CreateGPUBuffer(indexBufferSize, D3D12_RESOURCE_STATE_COMMON, dxContext.GetDeviceComPtr().Get());
            ComPtr<ID3D12Resource2> uploadBuffer =
                CreateUploadBuffer(uploadBufferSize, dxContext.GetDeviceComPtr().Get());
            ComPtr<ID3D12Resource2> constantBuffer =
                CreateUploadBuffer(constantBufferSize, dxContext.GetDeviceComPtr().Get());
            ComPtr<ID3D12Resource2> lightBuffer =
                CreateUploadBuffer(lightBufferSize, dxContext.GetDeviceComPtr().Get());

            CopyDataToResource(uploadBuffer.Get(), plane.mesh.vertices.data(), VecDataSize(plane.mesh.vertices));
            CopyDataToResource(
                uploadBuffer.Get(), plane.mesh.indices.data(), VecDataSize(plane.mesh.indices), vertexBufferSize
            );
            CopyDataToResource(lightBuffer.Get(), &lightBufferData, sizeof(LightBuffer));

            dxContext.ResetCmdList();
            QueueTransition(vertexBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST, barriers);
            QueueTransition(indexBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST, barriers);
            dxContext.GetCmdListComPtr()->ResourceBarrier(barriers.size(), barriers.data());
            barriers.clear();
            dxContext.GetCmdListComPtr()->CopyBufferRegion(
                vertexBuffer.Get(), 0, uploadBuffer.Get(), 0, VecDataSize(plane.mesh.vertices)
            );
            dxContext.GetCmdListComPtr()->CopyBufferRegion(
                indexBuffer.Get(), 0, uploadBuffer.Get(), vertexBufferSize, VecDataSize(plane.mesh.indices)
            );
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
                .SizeInBytes = static_cast<UINT>(VecDataSize(plane.mesh.vertices)),
                .StrideInBytes = sizeof(VertexBufferElement)
            };

            D3D12_INDEX_BUFFER_VIEW indexBufferView{
                .BufferLocation = indexBuffer->GetGPUVirtualAddress(),
                .SizeInBytes = static_cast<UINT>(VecDataSize(plane.mesh.indices)),
                .Format = DXGI_FORMAT_R32_UINT
            };

            Shader vertexShader("vertex_shader.cso");
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
            pipelineStateDesc.PS = {pixelShader.GetData(), pixelShader.GetSize()};
            pipelineStateDesc.DepthStencilState = depthStencilDesc;
            pipelineStateDesc.InputLayout = VB_INPUT_LAYOUT_DESC;
            ComPtr<ID3D12PipelineState> pipelineState;
            dxContext.GetDeviceComPtr()->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(&pipelineState));

            WindowClass balticWndClass(instance, TEXT("BalticWndClass"), GUIMsgQueueWindowProc);

            DXWindow mainWindow(instance, balticWndClass.GetAtom(), TEXT("Baltic"), 1920, 1080, dxContext);
            mainWindow.SetFullscreen(TRUE);
            GUI gui(mainWindow.GetWindowHandle(), dxContext.GetDeviceComPtr().Get());

            D3D12_DESCRIPTOR_HEAP_DESC dsvDescriptorHeapDesc{
                .Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
                .NumDescriptors = 1,
                .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
                .NodeMask = 0
            };

            ComPtr<ID3D12DescriptorHeap> dsvHeapDesc;
            DXThrowIfFailed(
                dxContext.GetDeviceComPtr()->CreateDescriptorHeap(&dsvDescriptorHeapDesc, IID_PPV_ARGS(&dsvHeapDesc))
            );

            D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeapDesc->GetCPUDescriptorHandleForHeapStart();

            ComPtr<ID3D12Resource2> dsBuffer = CreateDepthStencilBuffer(
                mainWindow.GetWidth(), mainWindow.GetHeight(), DSV_FORMAT, D3D12_RESOURCE_STATE_DEPTH_WRITE,
                dxContext.GetDeviceComPtr().Get()
            );

            D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{
                .Format = DSV_FORMAT, .ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D, .Flags = D3D12_DSV_FLAG_NONE
            };

            dxContext.GetDeviceComPtr().Get()->CreateDepthStencilView(dsBuffer.Get(), &dsvDesc, dsvHandle);

            std::unordered_map<WPARAM, BOOL> keyStates{
                {'W', FALSE}, {'A', FALSE}, {'S', FALSE}, {'D', FALSE}, {VK_SPACE, FALSE}, {VK_SHIFT, FALSE},
            };

            POINT lastCursorPos = mainWindow.GetCursorPosition();
            FLOAT xzPlaneAngle = 0.f;
            Camera camera(DirectX::XMMatrixIdentity());
            DirectX::XMMATRIX projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(
                DirectX::XMConvertToRadians(60.f), mainWindow.GetAspectRatio(), .1f, 100.f
            );
            auto prevFrameAbsTime = std::chrono::steady_clock::now();
            FLOAT absTimeMod2Pi = 0.f;
            BOOL close = FALSE;
            BOOL controlPanel = FALSE;
            BOOL focus = FALSE;

            while (!close) {
                FLOAT deltaTime = 0.f;
                auto currentFrameAbsTime = std::chrono::steady_clock::now();
                deltaTime = std::chrono::duration<FLOAT>(currentFrameAbsTime - prevFrameAbsTime).count();
                prevFrameAbsTime = currentFrameAbsTime;
                absTimeMod2Pi += deltaTime;
                absTimeMod2Pi = std::fmod(absTimeMod2Pi, DirectX::XM_2PI);

                mainWindow.Update();

                POINT mouseMovementVec = {.x = 0, .y = 0};

                WindowsMessage winMsg;
                while (!(winMsg = mainWindow.PollMsg()).empty) {
                    if (winMsg.msg == WM_CLOSE) {
                        close = TRUE;
                        break;
                    }
                    else if (winMsg.msg == WM_SIZE && LOWORD(winMsg.lParam) && HIWORD(winMsg.lParam) &&
                             (LOWORD(winMsg.lParam) != mainWindow.GetWidth() ||
                              HIWORD(winMsg.lParam) != mainWindow.GetHeight())) {
                        mainWindow.Resize(dxContext.GetDeviceComPtr().Get());

                        dsBuffer = CreateDepthStencilBuffer(
                            mainWindow.GetWidth(), mainWindow.GetHeight(), DSV_FORMAT, D3D12_RESOURCE_STATE_DEPTH_WRITE,
                            dxContext.GetDeviceComPtr().Get()
                        );
                        dxContext.GetDeviceComPtr().Get()->CreateDepthStencilView(dsBuffer.Get(), &dsvDesc, dsvHandle);

                        if (!controlPanel && focus) {
                            mainWindow.ConfineCursor();
                            mainWindow.CenterCursor();
                            lastCursorPos = mainWindow.GetCursorPosition();
                        }
                    }
                    else if (winMsg.msg == WM_SETFOCUS) {
                        focus = TRUE;
                        if (!controlPanel) {
                            mainWindow.SetCursorVisibility(FALSE);
                            mainWindow.ConfineCursor();
                            mainWindow.CenterCursor();
                            lastCursorPos = mainWindow.GetCursorPosition();
                        }
                    }
                    else if (winMsg.msg == WM_KILLFOCUS) {
                        focus = FALSE;
                        mainWindow.SetCursorVisibility(TRUE);
                        mainWindow.FreeCursor();
                        ResetKeyStates(keyStates);
                    }
                    else if (winMsg.msg == WM_MOVE) {
                        if (!controlPanel && focus) {
                            mainWindow.ConfineCursor();
                            mainWindow.CenterCursor();
                            lastCursorPos = mainWindow.GetCursorPosition();
                        }
                    }
                    else if (winMsg.msg == WM_KEYDOWN) {
                        if (winMsg.wParam == VK_F11) {
                            mainWindow.SetFullscreen(!mainWindow.isFullscreen());
                            if (!controlPanel) {
                                mainWindow.ConfineCursor();
                                mainWindow.CenterCursor();
                                lastCursorPos = mainWindow.GetCursorPosition();
                            }
                        }
                        else if (winMsg.wParam == VK_ESCAPE) {
                            controlPanel = !controlPanel;
                            if (!controlPanel) {
                                mainWindow.SetCursorVisibility(FALSE);
                                mainWindow.ConfineCursor();
                                mainWindow.CenterCursor();
                                lastCursorPos = mainWindow.GetCursorPosition();
                            }
                            else {
                                mainWindow.SetCursorVisibility(TRUE);
                                mainWindow.FreeCursor();
                                ResetKeyStates(keyStates);
                            }
                        }
                        else if (!controlPanel && focus && keyStates.contains(winMsg.wParam)) {
                            keyStates[winMsg.wParam] = TRUE;
                        }
                    }
                    else if (winMsg.msg == WM_KEYUP && keyStates.contains(winMsg.wParam)) {
                        keyStates[winMsg.wParam] = FALSE;
                    }
                    else if (winMsg.msg == WM_MOUSEMOVE) {
                        if (!controlPanel && focus) {
                            POINT mouseMovementVecAux = {
                                .x = LOWORD(winMsg.lParam) - lastCursorPos.x,
                                .y = HIWORD(winMsg.lParam) - lastCursorPos.y
                            };
                            mouseMovementVec.x += mouseMovementVecAux.x;
                            mouseMovementVec.y += mouseMovementVecAux.y;
                            mainWindow.CenterCursor();
                            lastCursorPos = mainWindow.GetCursorPosition();
                        }
                    }
                }

                camera.HandleInput(mouseMovementVec, keyStates, deltaTime);

                dxContext.ResetCmdList();

                const auto& cmdList = dxContext.GetCmdListComPtr();

                mainWindow.QueuePreRenderingTransitions(barriers);
                cmdList->ResourceBarrier(barriers.size(), barriers.data());
                barriers.clear();

                FLOAT clearColor[]{.1f, .1f, .1f, 1.f};
                cmdList->ClearRenderTargetView(*mainWindow.GetBackBufferRTVHandlePtr(), clearColor, 0, nullptr);
                cmdList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, nullptr);

                cmdList->OMSetRenderTargets(1, mainWindow.GetBackBufferRTVHandlePtr(), FALSE, &dsvHandle);

                cmdList->SetPipelineState(pipelineState.Get());
                cmdList->SetGraphicsRootSignature(rootSignature.GetComPtr().Get());

                cmdList->IASetVertexBuffers(0, 1, &vertexBufferView);
                cmdList->IASetIndexBuffer(&indexBufferView);
                cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

                cmdList->RSSetViewports(1, mainWindow.GetViewportPtr());
                cmdList->RSSetScissorRects(1, mainWindow.GetScissorRectPtr());

                ConstantBuffer constantBufferData{
                    .worldMatrix = plane.worldMatrix,
                    .viewMatrix = camera.GetViewMatrix(),
                    .projectionMatrix = projectionMatrix
                };

                CopyDataToResource(constantBuffer.Get(), &constantBufferData, sizeof(ConstantBuffer));

                lightBufferData.viewDirection = camera.GetViewDirection();

                CopyDataToResource(lightBuffer.Get(), &lightBufferData, sizeof(LightBuffer));

                cmdList->SetGraphicsRootConstantBufferView(0, constantBuffer->GetGPUVirtualAddress());
                cmdList->SetGraphicsRootConstantBufferView(1, lightBuffer->GetGPUVirtualAddress());
                cmdList->SetGraphicsRoot32BitConstants(2, 3, &plane.color, 0);
                cmdList->SetGraphicsRoot32BitConstant(3, std::bit_cast<UINT>(absTimeMod2Pi), 0);

                cmdList->DrawIndexedInstanced(plane.mesh.indices.size(), 1, 0, 0, 0);

                if (controlPanel) {
                    gui.QueueDraw(cmdList.Get());
                }

                mainWindow.QueuePostRenderingTransitions(barriers);
                cmdList->ResourceBarrier(barriers.size(), barriers.data());
                barriers.clear();

                dxContext.ExecuteCmdList();

                mainWindow.Present();

                dxContext.Flush(FRAME_COUNT);
            }
        }

        dxDebugLayer.ReportLiveObjects();
    }
    catch (const BalticException& e) {
#ifdef UNICODE
        OutputDebugStringW((L"Error: " + std::wstring(e.GetMessage())).c_str());
#else
        OutputDebugStringA(("Error: " + std::string(e.GetMessage())).c_str());
#endif
        ret = 1;
    }

    return ret;
}

// Implementations of auxiliary functions
SIZE_T AlignUp(SIZE_T size, SIZE_T alignment) { return (size + alignment - 1) & ~(alignment - 1); }

Mesh CreateXZPlane(FLOAT xSize, FLOAT zSize, UINT32 xSegments, UINT32 zSegments)
{
    std::vector<VertexBufferElement> vertices;
    std::vector<UINT32> indices;

    FLOAT xStep = xSize / xSegments;
    FLOAT zStep = zSize / zSegments;

    for (UINT32 i = 0; i <= zSegments; i++) {
        for (UINT32 j = 0; j <= xSegments; j++) {
            vertices.push_back({.position{j * xStep, 0.f, i * zStep}});
        }
    }

    for (UINT32 i = 0; i < zSegments; i++) {
        for (UINT32 j = 0; j < xSegments; j++) {
            UINT32 index = i * (xSegments + 1) + j;
            indices.push_back(index);
            indices.push_back(index + xSegments + 1);
            indices.push_back(index + 1);
            indices.push_back(index + 1);
            indices.push_back(index + xSegments + 1);
            indices.push_back(index + xSegments + 2);
        }
    }

    return {.vertices = vertices, .indices = indices};
}

void ResetKeyStates(std::unordered_map<WPARAM, BOOL>& keyStates)
{
    for (auto& [key, state] : keyStates) {
        state = FALSE;
    }
}

template <typename T>
SIZE_T VecDataSize(const std::vector<T>& vec)
{
    return vec.size() * sizeof(T);
}
