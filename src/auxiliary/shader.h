#pragma once

#include <string_view>

namespace Baltic
{
    class Shader
    {
    public:
        Shader(std::string_view shaderPath);
        ~Shader();

        inline const void* GetData() const { return m_data; }
        inline size_t GetSize() const { return m_size; }

    private:
        void* m_data;
        size_t m_size;
    };

}  // namespace Baltic
