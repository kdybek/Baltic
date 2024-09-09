#pragma once

#include <vector>

namespace Baltic
{
    using Microsoft::WRL::ComPtr;

    struct VertexBufferElement
    {
        DirectX::XMFLOAT3 position;

    } __attribute__((packed));

    struct CameraCBuffer
    {
        DirectX::XMMATRIX viewMatrix;
        DirectX::XMMATRIX projectionMatrix;

    } __attribute__((packed));

    struct Mesh
    {
        std::vector<VertexBufferElement> vertices;
        std::vector<UINT32> indices;
    };

    enum class WindowEvent
    {
        Resize,
        Close
    };

} // namespace Baltic
