#pragma once

#include "auxiliary/win_include.h"

namespace Baltic
{
    class UploadBuffer
    {
    public:
        UploadBuffer(UINT64 size, ID3D12Device5* device);
        ~UploadBuffer() = default;

        UploadBuffer(const UploadBuffer&) = delete;
        UploadBuffer& operator=(const UploadBuffer&) = delete;

        void CopyData(const void* data, UINT size) const;
        void StageCmdUpload(ID3D12Resource2* dest, UINT size, ID3D12GraphicsCommandList6* cmdList) const;

        [[nodiscard]] inline const ComPtr<ID3D12Resource2>& GetComPtr() { return m_uploadBuffer; }

    private:
        ComPtr<ID3D12Resource2> m_uploadBuffer;
    };

    class GPUBuffer
    {
    public:
        GPUBuffer(UINT64 size, ID3D12Device5* device);
        ~GPUBuffer() = default;

        GPUBuffer(const GPUBuffer&) = delete;
        GPUBuffer& operator=(const GPUBuffer&) = delete;

        [[nodiscard]] inline const ComPtr<ID3D12Resource2>& GetComPtr() { return m_buffer; }

    private:
        ComPtr<ID3D12Resource2> m_buffer;
    };

} // namespace Baltic
