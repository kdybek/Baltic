#pragma once

#include "auxiliary/win_include.h"

namespace Baltic
{
    class DXContext
    {
    public:
        DXContext();

        ~DXContext();

        DXContext(const DXContext&) = delete;

        DXContext& operator=(const DXContext&) = delete;

        void SignalAndWait();

        void ResetCmdList();

        void ExecuteCmdList();

        void Flush(UINT count);

        [[nodiscard]] inline const Microsoft::WRL::ComPtr<IDXGIFactory7>& GetFactory() const { return m_factory; }

        [[nodiscard]] inline const Microsoft::WRL::ComPtr<ID3D12Device8>& GetDevice() const { return m_device; }

        [[nodiscard]] inline const Microsoft::WRL::ComPtr<ID3D12CommandQueue>& GetCmdQueue() const
        {
            return m_cmdQueue;
        }

        [[nodiscard]] inline const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList6>& GetCmdList() const
        {
            return m_cmdList;
        }

    private:
        Microsoft::WRL::ComPtr<IDXGIFactory7> m_factory;

        Microsoft::WRL::ComPtr<ID3D12Device8> m_device;
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_cmdQueue;

        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_cmdAllocator;
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList6> m_cmdList;

        Microsoft::WRL::ComPtr<ID3D12Fence1> m_fence;
        UINT64 m_fenceValue;
        HANDLE m_fenceEvent;
    };

}  // namespace Baltic
