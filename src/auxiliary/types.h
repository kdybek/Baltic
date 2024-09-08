#pragma once

#include <vector>

#include "auxiliary/input_layouts.h"

namespace Baltic
{
    struct Mesh
    {
        std::vector<VertexBufferElement> vertices;
        std::vector<UINT32> indices;
    };

} // namespace Baltic
