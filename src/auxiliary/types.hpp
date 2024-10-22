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

struct alignas(16) ConstantBuffer
{
    DirectX::XMMATRIX viewMatrix;
    DirectX::XMMATRIX projectionMatrix;
    DirectX::XMFLOAT3 color;
    FLOAT padding;

};

struct alignas(16) LightSource
{
    DirectX::XMFLOAT3 position;
    FLOAT padding;
    DirectX::XMFLOAT3 color;
    FLOAT intensity;

};

struct alignas(16) LightBuffer
{
    DirectX::XMFLOAT3 sunlightDirection;
    FLOAT padding;
    DirectX::XMFLOAT3 sunlightColor;
    FLOAT sunlightIntensity;
    DirectX::XMFLOAT3 viewDirection;
    FLOAT padding2;

};

struct alignas(16) ModelBuffer
{
    DirectX::XMMATRIX worldMatrix;
    DirectX::XMFLOAT3 color;
    FLOAT ambientIntensity;
    FLOAT diffuseIntensity;
    FLOAT specularIntensity;
    FLOAT specularPower;
    FLOAT padding;

};

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
