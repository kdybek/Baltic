#pragma once

#include <vector>

#include "auxiliary/constants.h"

namespace Baltic
{
    using Microsoft::WRL::ComPtr;

    struct VertexBufferElement
    {
        DirectX::XMFLOAT3 position;
        FLOAT padding;

    } __attribute__((packed));

    struct CameraCBuffer
    {
        DirectX::XMMATRIX viewMatrix;
        DirectX::XMMATRIX projectionMatrix;

    } __attribute__((packed));

    struct LightSource
    {
        DirectX::XMFLOAT3 position;
        FLOAT padding;
        DirectX::XMFLOAT3 color;
        FLOAT intensity;

    } __attribute__((packed));

    struct LightCBuffer
    {
        LightSource lightSources[MAX_LIGHTS];
        UINT32 numLights;
        FLOAT padding[3];

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
        Space,
        Shift,
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
