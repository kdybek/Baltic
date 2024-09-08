#pragma once

#include <vector>

#include "auxiliary/input_layouts.h"

namespace Baltic
{
    struct Model
    {
        std::vector<VertexBufferElement> vertices;
        std::vector<UINT32> indices;
    };

} // namespace Baltic
