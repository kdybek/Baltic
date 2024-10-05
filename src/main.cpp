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

// Auxiliary functions
SIZE_T AlignUp(SIZE_T size, SIZE_T alignment) { return (size + alignment - 1) & ~(alignment - 1); }

template <typename T>
SIZE_T VecDataSize(const std::vector<T>& vec)
{
    return vec.size() * sizeof(T);
}

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

// Auxiliary state modules
class Camera
{
public:
    Camera() = default;

    Camera(const DirectX::XMMATRIX& viewMatrix) : m_viewMatrix(viewMatrix), m_xzPlaneAngle(0.f) {}

    void HandleInput(POINT mouseMovementVec, const std::unordered_map<Key, BOOL>& keyStates)
    {
        FLOAT xAngle = mouseMovementVec.y * -m_mouseSensitivity;
        FLOAT yAngle = mouseMovementVec.x * -m_mouseSensitivity;
        FLOAT xTranslation = 0.f;
        FLOAT yTranslation = 0.f;
        FLOAT zTranslation = 0.f;

        if (keyStates.at(Key::W)) {
            zTranslation -= m_translationSpeed;
        }

        if (keyStates.at(Key::A)) {
            xTranslation += m_translationSpeed;
        }

        if (keyStates.at(Key::S)) {
            zTranslation += m_translationSpeed;
        }

        if (keyStates.at(Key::D)) {
            xTranslation -= m_translationSpeed;
        }

        if (keyStates.at(Key::Space)) {
            yTranslation -= m_translationSpeed;
        }

        if (keyStates.at(Key::Shift)) {
            yTranslation += m_translationSpeed;
        }

        if (xAngle + m_xzPlaneAngle > DirectX::XM_PIDIV2) {
            xAngle = DirectX::XM_PIDIV2 - m_xzPlaneAngle;
        }
        else if (xAngle + m_xzPlaneAngle < -DirectX::XM_PIDIV2) {
            xAngle = -DirectX::XM_PIDIV2 - m_xzPlaneAngle;
        }

        DirectX::XMMATRIX rotationMatrix1 = DirectX::XMMatrixRotationX(-m_xzPlaneAngle);
        DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslation(xTranslation, yTranslation, zTranslation);
        DirectX::XMMATRIX rotationMatrix2 = DirectX::XMMatrixRotationY(yAngle);
        DirectX::XMMATRIX rotationMatrix3 = DirectX::XMMatrixRotationX(xAngle + m_xzPlaneAngle);

        m_viewMatrix = DirectX::XMMatrixMultiply(m_viewMatrix, rotationMatrix1);
        m_viewMatrix = DirectX::XMMatrixMultiply(m_viewMatrix, translationMatrix);
        m_viewMatrix = DirectX::XMMatrixMultiply(m_viewMatrix, rotationMatrix2);
        m_viewMatrix = DirectX::XMMatrixMultiply(m_viewMatrix, rotationMatrix3);

        m_xzPlaneAngle += xAngle;
    }

    [[nodiscard]] DirectX::XMMATRIX GetViewMatrix() const { return m_viewMatrix; }

private:
    DirectX::XMMATRIX m_viewMatrix;
    FLOAT m_xzPlaneAngle;
    FLOAT m_mouseSensitivity = .001f;
    FLOAT m_translationSpeed = .1f;
};

int main()
{
    int ret = 0;

    try {
        DXThrowIfFailed(RoInitialize(RO_INIT_SINGLETHREADED));

        DXDebugLayer dxDebugLayer;

        {
            DXContext dxContext;

            std::vector<D3D12_RESOURCE_BARRIER> barriers;

            Model plane = {
                .mesh = CreateXZPlane(10.f, 10.f, 20, 20),
                .worldMatrix = DirectX::XMMatrixTranslation(0.f, -4.f, 0.f),
                .color{1.f, 1.f, 1.f}
            };

            LightSource lightSource1{.position{-1.f, 1.f, 0.f}, .color{0.4f, 1.f, 1.f}, .intensity = .9f};
            LightSource lightSource2{.position{1.f, -1.f, 3.f}, .color{1.f, 0.4f, 1.f}, .intensity = .9f};
            LightBuffer lightBufferData{.lightSources{lightSource1, lightSource2}, .numLights = 2};

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

            std::unordered_map<Key, BOOL> keyStates{{Key::W, FALSE},  {Key::A, FALSE},     {Key::S, FALSE},
                                                    {Key::D, FALSE},  {Key::Space, FALSE}, {Key::Shift, FALSE},
                                                    {Key::F11, FALSE}};

            BOOL end = FALSE;
            POINT lastCursorPos = mainWindow.GetCursorPosition();
            FLOAT xzPlaneAngle = 0.f;
            Camera camera(DirectX::XMMatrixIdentity());
            DirectX::XMMATRIX projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(
                DirectX::XMConvertToRadians(60.f), mainWindow.GetAspectRatio(), .1f, 100.f
            );

            while (!end) {
                mainWindow.Update();
                POINT mouseMovementVec = {.x = 0, .y = 0};

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
                        POINT mouseMovementVecAux = {
                            .x = event.cursorPosition.x - lastCursorPos.x, .y = event.cursorPosition.y - lastCursorPos.y
                        };
                        mouseMovementVec.x += mouseMovementVecAux.x;
                        mouseMovementVec.y += mouseMovementVecAux.y;
                        mainWindow.CenterCursor();
                        lastCursorPos = mainWindow.GetCursorPosition();
                    }
                }

                camera.HandleInput(mouseMovementVec, keyStates);

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

                ConstantBuffer constantBufferData{
                    .worldMatrix = plane.worldMatrix,
                    .viewMatrix = camera.GetViewMatrix(),
                    .projectionMatrix = projectionMatrix
                };

                CopyDataToResource(constantBuffer.Get(), &constantBufferData, sizeof(ConstantBuffer));

                cmdList->SetGraphicsRootConstantBufferView(0, constantBuffer->GetGPUVirtualAddress());
                cmdList->SetGraphicsRootConstantBufferView(1, lightBuffer->GetGPUVirtualAddress());
                cmdList->SetGraphicsRoot32BitConstants(2, 3, &plane.color, 0);

                cmdList->DrawIndexedInstanced(plane.mesh.indices.size(), 1, 0, 0, 0);

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
