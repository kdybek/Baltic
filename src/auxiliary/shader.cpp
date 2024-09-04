#include "shader.h"

#include <filesystem>
#include <fstream>

#include "auxiliary/baltic_except.h"

namespace Baltic
{
    Shader::Shader(std::string_view filename) : m_data(nullptr), m_size(0)
    {
        static std::filesystem::path s_shaderDir;

        if (s_shaderDir.empty()) {
            wchar_t exePath[MAX_PATH];
            if (!GetModuleFileNameW(nullptr, exePath, MAX_PATH)) {
                throw BalticException("GetModuleFileNameW");
            }

            s_shaderDir = exePath;
            s_shaderDir.remove_filename();
        }

        std::ifstream shaderIn(s_shaderDir / filename, std::ios::binary);

        if (!shaderIn.is_open()) {
            throw BalticException("Failed to open shader file");
        }

        shaderIn.seekg(0, std::ios::end);
        m_size = shaderIn.tellg();
        shaderIn.seekg(0, std::ios::beg);
        if (!(m_data = malloc(m_size))) {
            throw BalticException("malloc");
        }

        shaderIn.read(static_cast<char*>(m_data), m_size);
    }

    Shader::~Shader()
    {
        if (m_data) {
            free(m_data);
        }
    }

}  // namespace Baltic
