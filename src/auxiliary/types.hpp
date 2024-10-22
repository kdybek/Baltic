#pragma once

// clang-format off
#include "auxiliary/pch.hpp"
// clang-format on

#include "auxiliary/constants.hpp"

using Microsoft::WRL::ComPtr;

struct VertexBufferElement
{
    DirectX::XMFLOAT3 position;
};

struct ConstantBuffer
{
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
    DirectX::XMFLOAT3 sunlightDirection;
    FLOAT padding;
    DirectX::XMFLOAT3 sunlightColor;
    FLOAT sunlightIntensity;
    DirectX::XMFLOAT3 viewDirection;
    FLOAT padding2;

} __attribute__((packed));

struct ModelBuffer
{
    DirectX::XMMATRIX worldMatrix;
    DirectX::XMFLOAT3 color;
    FLOAT ambientIntensity;
    FLOAT diffuseIntensity;
    FLOAT specularIntensity;
    FLOAT specularPower;
    FLOAT padding;

} __attribute__((packed));

struct Mesh
{
    std::vector<VertexBufferElement> vertices;
    std::vector<UINT32> indices;
};

struct Model
{
    const CHAR* name;
    Mesh mesh;
    ModelBuffer modelBuffer;
};

struct WindowsMessage
{
    UINT msg;
    WPARAM wParam;
    LPARAM lParam;
    BOOL empty;
};
