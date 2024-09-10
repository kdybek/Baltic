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

    enum class EventType
    {
        None,
        Close,
        Resize,
        Focus,
        Blur,
        Move,
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
        F11
    };

    struct Event
    {
        EventType type;
        union
        {
            Key key;
            POINT cursorPosition;
        };
    };

} // namespace Baltic
