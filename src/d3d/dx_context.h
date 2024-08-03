#pragma once

#include "../auxiliary/win_include.h"
#include "../baltic_except.h"

namespace Baltic
{

    class DXContext
    {
    public:
        DXContext(const DXContext&) = delete;

        DXContext& operator=(const DXContext&) = delete;

        inline static DXContext& Get()
        {
            static DXContext s_dxContext;
            return s_dxContext;
        }

        inline const Microsoft::WRL::ComPtr<ID3D12Device8>& GetDevice() {
            return m_device;
        }

        inline const Microsoft::WRL::ComPtr<ID3D12CommandQueue>& GetCmdQueue() {
            return m_cmdQueue;
        }

    private:
        DXContext();

    private:
        Microsoft::WRL::ComPtr<ID3D12Device8> m_device;
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_cmdQueue;
        Microsoft::WRL::ComPtr<ID3D12Fence1> m_fence;
        UINT64 m_fenceValue;

    };

} // Baltic
