#pragma once

#include "auxiliary/win_include.h"

namespace Baltic
{
    using DXGIFactory7ComPtr = Microsoft::WRL::ComPtr<IDXGIFactory7>;
    using DXGISwapChain1ComPtr = Microsoft::WRL::ComPtr<IDXGISwapChain1>;
    using DXGISwapChain4ComPtr = Microsoft::WRL::ComPtr<IDXGISwapChain4>;
    using DXGIDebug1ComPtr = Microsoft::WRL::ComPtr<IDXGIDebug1>;

    using D3D12Device8ComPtr = Microsoft::WRL::ComPtr<ID3D12Device8>;
    using D3D12CommandQueueComPtr = Microsoft::WRL::ComPtr<ID3D12CommandQueue>;
    using D3D12CommandAllocatorComPtr = Microsoft::WRL::ComPtr<ID3D12CommandAllocator>;
    using D3D12GraphicsCommandList6ComPtr = Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList6>;
    using D3D12Fence1ComPtr = Microsoft::WRL::ComPtr<ID3D12Fence1>;
    using D3D12Resource2ComPtr = Microsoft::WRL::ComPtr<ID3D12Resource2>;
    using D3D12DescriptorHeapComPtr = Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>;
    using D3D12RootSignatureComPtr = Microsoft::WRL::ComPtr<ID3D12RootSignature>;
    using D3D12PipelineStateComPtr = Microsoft::WRL::ComPtr<ID3D12PipelineState>;
    using D3D12Debug3ComPtr = Microsoft::WRL::ComPtr<ID3D12Debug3>;

}  // namespace Baltic
