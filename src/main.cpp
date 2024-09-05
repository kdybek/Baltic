#include <directxmath.h>

#include <iostream>

#include "auxiliary/baltic_except.h"
#include "auxiliary/constants.h"
#include "auxiliary/dx_window.h"
#include "auxiliary/shader.h"
#include "auxiliary/win_include.h"
#include "d3d/dx_context.h"
#include "d3d/pipeline_state.h"
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
            D3D12_HEAP_PROPERTIES heapPropertiesUpload{
                .Type = D3D12_HEAP_TYPE_UPLOAD,
                .CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
                .MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
                .CreationNodeMask = 0,
                .VisibleNodeMask = 0
            };

            D3D12_HEAP_PROPERTIES heapPropertiesDefault{
                .Type = D3D12_HEAP_TYPE_DEFAULT,
                .CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
                .MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
                .CreationNodeMask = 0,
                .VisibleNodeMask = 0
            };

            DirectX::XMFLOAT2 vertices[3] = {{0.0f, 0.5f}, {0.5f, -0.5f}, {-0.5f, -0.5f}};

            D3D12_INPUT_ELEMENT_DESC vertexLayout[] = {
                {"Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
            };

            D3D12_RESOURCE_DESC resourceDesc{
                .Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
                .Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT,
                .Width = 1024,
                .Height = 1,
                .DepthOrArraySize = 1,
                .MipLevels = 1,
                .Format = DXGI_FORMAT_UNKNOWN,
                .SampleDesc{.Count = 1, .Quality = 0},
                .Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
                .Flags = D3D12_RESOURCE_FLAG_NONE
            };

            ComPtr<ID3D12Resource2> uploadBuffer, vertexBuffer;

            DXContext dxContext;

            if (FAILED(dxContext.GetDeviceComPtr()->CreateCommittedResource(
                    &heapPropertiesUpload, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
                    nullptr, IID_PPV_ARGS(&uploadBuffer)
                ))) {
                throw BalticException("dxContext.GetDevice()->CreateCommittedResource");
            }

            if (FAILED(dxContext.GetDeviceComPtr()->CreateCommittedResource(
                    &heapPropertiesDefault, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr,
                    IID_PPV_ARGS(&vertexBuffer)
                ))) {
                throw BalticException("dxContext.GetDevice()->CreateCommittedResource");
            }

            D3D12_VERTEX_BUFFER_VIEW vertexBufferView{
                .BufferLocation = vertexBuffer->GetGPUVirtualAddress(),
                .SizeInBytes = sizeof(DirectX::XMFLOAT2) * _countof(vertices),
                .StrideInBytes = sizeof(DirectX::XMFLOAT2)
            };

            void* uploadBufferAddr;
            D3D12_RANGE uploadRange{.Begin = 0, .End = 1023};

            if (FAILED(uploadBuffer->Map(0, &uploadRange, &uploadBufferAddr))) {
                throw BalticException("uploadBuffer->Map");
            }
            memcpy(uploadBufferAddr, vertices, sizeof(vertices));
            uploadBuffer->Unmap(0, &uploadRange);

            dxContext.ResetCmdList();

            dxContext.GetCmdListComPtr()->CopyBufferRegion(vertexBuffer.Get(), 0, uploadBuffer.Get(), 0, 1024);
            dxContext.ExecuteCmdList();

            Shader vertexShader("vertex_shader.cso");
            Shader pixelShader("pixel_shader.cso");
            Shader rootSignatureShader("root_signature.cso");

            ComPtr<ID3D12RootSignature> rootSignature;
            dxContext.GetDeviceComPtr()->CreateRootSignature(
                0, rootSignatureShader.GetData(), rootSignatureShader.GetSize(), IID_PPV_ARGS(&rootSignature)
            );

            PipelineState pipelineState;

            pipelineState.StageRootSignature(rootSignature);
            pipelineState.StageVertexShader(vertexShader);
            pipelineState.StagePixelShader(pixelShader);
            pipelineState.StageInputLayout({vertexLayout, _countof(vertexLayout)});
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

                mainWindow.BeginFrame(cmdList.Get());

                cmdList->SetPipelineState(pipelineState.GetComPtr().Get());
                cmdList->SetGraphicsRootSignature(rootSignature.Get());

                cmdList->IASetVertexBuffers(0, 1, &vertexBufferView);
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

                mainWindow.EndFrame(cmdList.Get());

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
