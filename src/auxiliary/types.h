#pragma once

#include <vector>

#include "auxiliary/constants.h"

namespace Baltic
{
    using Microsoft::WRL::ComPtr;

    struct VertexBufferElement
    {
        DirectX::XMFLOAT3 position;
    };

    struct ConstantBuffer
    {
        DirectX::XMMATRIX worldMatrix;
        DirectX::XMMATRIX viewMatrix;
        DirectX::XMMATRIX projectionMatrix;
        DirectX::XMFLOAT3 color;
        FLOAT padding;

    } __attribute__((packed));

    struct LightSource
    {
        DirectX::XMFLOAT3 position;
        FLOAT padding;
        DirectX::XMFLOAT3 color;
        FLOAT intensity;

    } __attribute__((packed));

    struct LightBuffer
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

    struct Model
    {
        Mesh mesh;
        DirectX::XMMATRIX worldMatrix;
        DirectX::XMFLOAT3 color;
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
