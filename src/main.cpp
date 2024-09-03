#include <directxmath.h>

#include <iostream>

#include "auxiliary/baltic_except.h"
#include "auxiliary/constants.h"
#include "auxiliary/dx_window.h"
#include "d3d/dx_context.h"
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
            {"Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},

            {"Color", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
             D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
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

        Microsoft::WRL::ComPtr<ID3D12Resource2> uploadBuffer, vertexBuffer;

        DXContext dxContext;

        if (FAILED(dxContext.GetDevice()->CreateCommittedResource(
                &heapPropertiesUpload, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                IID_PPV_ARGS(&uploadBuffer)
            ))) {
            throw BalticException("dxContext.GetDevice()->CreateCommittedResource");
        }

        if (FAILED(dxContext.GetDevice()->CreateCommittedResource(
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

        const auto& cmdList = dxContext.GetCmdList();
        cmdList->CopyBufferRegion(vertexBuffer.Get(), 0, uploadBuffer.Get(), 0, 1024);
        dxContext.ExecuteCmdList();

        D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc{
            .InputLayout{
                .pInputElementDescs = vertexLayout,
                .NumElements = _countof(vertexLayout),
            },
            .IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED,
        };

        DXWindow mainWindow(dxContext, 1920, 1080);
        mainWindow.SetFullscreen(TRUE);

        while (!mainWindow.ShouldClose()) {
            mainWindow.Update();

            if (mainWindow.ShouldResize()) {
                dxContext.Flush(FRAME_COUNT);
                mainWindow.ResizeSwapChain();
            }

            dxContext.ResetCmdList();

            const auto& cmdList1 = dxContext.GetCmdList();

            mainWindow.BeginFrame(cmdList1.Get());

            cmdList1->IASetVertexBuffers(0, 1, &vertexBufferView);
            cmdList1->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            cmdList1->DrawInstanced(_countof(vertices), 1, 0, 0);

            mainWindow.EndFrame(cmdList1.Get());

            dxContext.ExecuteCmdList();

            mainWindow.Present();
        }

        dxContext.Flush(FRAME_COUNT);
    }
    catch (const BalticException& e) {
        std::cout << e.what() << '\n';
        ret = 1;
    }

    RoUninitialize();

    return ret;
}
