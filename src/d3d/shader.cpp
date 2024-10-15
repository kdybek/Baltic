#include "shader.hpp"

#include "auxiliary/baltic_exception.hpp"

Shader::Shader(std::string_view filename) : m_data(nullptr), m_size(0)
{
    static std::filesystem::path s_shaderDir;

    if (s_shaderDir.empty()) {
        TCHAR exePath[512];
        if (!GetModuleFileName(nullptr, exePath, 512)) {
            throw GenericException(TEXT("GetModuleFileName"));
        }

        s_shaderDir = exePath;
        s_shaderDir.remove_filename();
    }

    std::ifstream shaderIn(s_shaderDir / filename, std::ios::binary);

    if (!shaderIn.is_open()) {
        throw GenericException(TEXT("Failed to open shader file"));
    }

    shaderIn.seekg(0, std::ios::end);
    m_size = shaderIn.tellg();
    shaderIn.seekg(0, std::ios::beg);
    if (!(m_data = malloc(m_size))) {
        throw GenericException(TEXT("malloc"));
    }

    shaderIn.read(static_cast<char*>(m_data), m_size);
}

Shader::~Shader()
{
    if (m_data) {
        free(m_data);
    }
}

RootSignature::RootSignature(std::string_view filename, ID3D12Device5* device)
{
    Shader rootSigShader(filename);

    DXThrowIfFailed(
        device->CreateRootSignature(0, rootSigShader.GetData(), rootSigShader.GetSize(), IID_PPV_ARGS(&m_rootSignature))
    );
}
