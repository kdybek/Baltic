#pragma once

#include <string_view>

#include "auxiliary/win_include.h"

namespace Baltic
{
    class Shader
    {
    public:
        Shader(std::string_view shaderPath);
        ~Shader();

        inline const void* GetData() const { return m_data; }
        inline SIZE_T GetSize() const { return m_size; }

    private:
        void* m_data;
        SIZE_T m_size;
    };

} // namespace Baltic
