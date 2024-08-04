#pragma once

#include "auxiliary/win_include.h"
#include "auxiliary/baltic_except.h"

namespace Baltic
{
    class DXContext
    {
    public:
        ~DXContext();

        DXContext(const DXContext&) = delete;

        DXContext& operator=(const DXContext&) = delete;

        void SignalAndWait();

        void ResetCmdList();

        void ExecuteCmdList();

        inline static DXContext& Get()
        {
            static DXContext s_dxContext;
            return s_dxContext;
        }

        inline const Microsoft::WRL::ComPtr<ID3D12Device8>& GetDevice()
        {
            return m_device;
        }

        inline const Microsoft::WRL::ComPtr<ID3D12CommandQueue>& GetCmdQueue()
        {
            return m_cmdQueue;
        }

        inline const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList6>& GetCmdList()
        {
            return m_cmdList;
        }

    private:
        DXContext();

    private:
        Microsoft::WRL::ComPtr<ID3D12Device8> m_device;
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_cmdQueue;

        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_cmdAllocator;
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList6> m_cmdList;

        Microsoft::WRL::ComPtr<ID3D12Fence1> m_fence;
        UINT64 m_fenceValue;
        HANDLE m_fenceEvent;

    };

} // Baltic
