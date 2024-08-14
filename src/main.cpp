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

        DXDebugLayer::Init();

        const char* str = "abcdefg";

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

        if (FAILED(DXContext::Get().GetDevice()->CreateCommittedResource(
                &heapPropertiesUpload, D3D12_HEAP_FLAG_NONE, &resourceDesc,
                D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                IID_PPV_ARGS(&uploadBuffer)
            ))) {
            throw BalticException(
                "DXContext::Get().GetDevice()->CreateCommittedResource"
            );
        }

        if (FAILED(DXContext::Get().GetDevice()->CreateCommittedResource(
                &heapPropertiesDefault, D3D12_HEAP_FLAG_NONE, &resourceDesc,
                D3D12_RESOURCE_STATE_COMMON, nullptr,
                IID_PPV_ARGS(&vertexBuffer)
            ))) {
            throw BalticException(
                "DXContext::Get().GetDevice()->CreateCommittedResource"
            );
        }

        void* uploadBufferAddr;
        D3D12_RANGE uploadRange{.Begin = 0, .End = 1023};

        if (FAILED(uploadBuffer->Map(0, &uploadRange, &uploadBufferAddr))) {
            throw BalticException("uploadBuffer->Map");
        }
        memcpy(uploadBufferAddr, str, strlen(str));
        uploadBuffer->Unmap(0, &uploadRange);

        DXContext::Get().ResetCmdList();

        const auto& cmdList = DXContext::Get().GetCmdList();
        cmdList->CopyBufferRegion(
            vertexBuffer.Get(), 0, uploadBuffer.Get(), 0, 1024
        );
        DXContext::Get().ExecuteCmdList();

        DXWindow mainWindow;
        mainWindow.SetFullscreen(TRUE);

        while (!mainWindow.ShouldClose()) {
            mainWindow.Update();

            if (mainWindow.ShouldResize()) {
                DXContext::Get().Flush(FRAME_COUNT);
                mainWindow.ResizeSwapChain();
            }

            DXContext::Get().ResetCmdList();

            const auto& cmdList1 = DXContext::Get().GetCmdList();

            mainWindow.BeginFrame(cmdList1.Get());
            // Draw
            mainWindow.EndFrame(cmdList1.Get());

            DXContext::Get().ExecuteCmdList();

            mainWindow.Present();
        }

        DXContext::Get().Flush(FRAME_COUNT);
    }
    catch (const BalticException& e) {
        std::cout << e.what() << '\n';
        ret = 1;
    }

    RoUninitialize();

    return ret;
}
