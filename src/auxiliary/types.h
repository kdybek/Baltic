#pragma once

#include "auxiliary/win_include.h"

namespace Baltic
{
    using DXGIFactory7ComPtr = Microsoft::WRL::ComPtr<IDXGIFactory7>;
    using DXDevice8ComPtr = Microsoft::WRL::ComPtr<ID3D12Device8>;
    using DXCommandQueueComPtr = Microsoft::WRL::ComPtr<ID3D12CommandQueue>;
    using DXCommandAllocatorComPtr = Microsoft::WRL::ComPtr<ID3D12CommandAllocator>;
    using DXGraphicsCommandList6ComPtr = Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList6>;
    using DXFence1ComPtr = Microsoft::WRL::ComPtr<ID3D12Fence1>;
    using DXSwapChain1ComPtr = Microsoft::WRL::ComPtr<IDXGISwapChain1>;
    using DXSwapChain4ComPtr = Microsoft::WRL::ComPtr<IDXGISwapChain4>;
    using DXResource2ComPtr = Microsoft::WRL::ComPtr<ID3D12Resource2>;
    using DXDescriptorHeapComPtr = Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>;
    using DXRootSignatureComPtr = Microsoft::WRL::ComPtr<ID3D12RootSignature>;
    using DXPipelineStateComPtr = Microsoft::WRL::ComPtr<ID3D12PipelineState>;
    using D3D12Debug3ComPtr = Microsoft::WRL::ComPtr<ID3D12Debug3>;
    using DXGIDebug1ComPtr = Microsoft::WRL::ComPtr<IDXGIDebug1>;

}  // namespace Baltic
