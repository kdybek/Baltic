#pragma once

// clang-format off
#include "auxiliary/win_include.h"
// clang-format on

#include <string_view>

namespace Baltic
{
    class Shader
    {
    public:
        Shader(std::string_view filename);
        ~Shader();

        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;

        [[nodiscard]] inline const void* GetData() const { return m_data; }
        [[nodiscard]] inline SIZE_T GetSize() const { return m_size; }

    private:
        void* m_data;
        SIZE_T m_size;
    };

    class RootSignature
    {
    public:
        RootSignature(std::string_view filename, ID3D12Device5* device);
        ~RootSignature() = default;
        
        RootSignature(const RootSignature&) = delete;
        RootSignature& operator=(const RootSignature&) = delete;

        [[nodiscard]] inline const ComPtr<ID3D12RootSignature>& GetComPtr() { return m_rootSignature; }

    private:
        ComPtr<ID3D12RootSignature> m_rootSignature;
    };

} // namespace Baltic
