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

    enum class WindowEventType
    {
        None,
        Resize,
        Close,
        KeyDown,
        KeyUp,
        MouseMove
    };

    enum class Key
    {
        W,
        A,
        S,
        D,
    };

    struct WindowEvent
    {
        WindowEventType type;
        union
        {
            Key key;
            struct
            {
                SHORT x;
                SHORT y;
            } position;
        };
    };

} // namespace Baltic
