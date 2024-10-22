#pragma once

// clang-format off
#include "auxiliary/pch.hpp"
// clang-format on

class Camera
{
public:
    inline Camera(const DirectX::XMMATRIX& viewMatrix)
        : m_viewMatrix(viewMatrix), m_xzPlaneAngle(0.f), m_rotationSpeed(.04f), m_movementSpeed(4.f)
    {
    }

    void inline HandleInput(POINT mouseMovementVec, const std::unordered_map<WPARAM, BOOL>& keyStates, FLOAT deltaTime)
    {
        FLOAT perUnitAngle = m_rotationSpeed * deltaTime;
        FLOAT perUnitDistance = m_movementSpeed * deltaTime;
        FLOAT xAngle = mouseMovementVec.y * -perUnitAngle;
        FLOAT yAngle = mouseMovementVec.x * -perUnitAngle;
        FLOAT xTranslation = 0.f;
        FLOAT yTranslation = 0.f;
        FLOAT zTranslation = 0.f;

        if (keyStates.at('W')) {
            zTranslation -= perUnitDistance;
        }

        if (keyStates.at('A')) {
            xTranslation += perUnitDistance;
        }

        if (keyStates.at('S')) {
            zTranslation += perUnitDistance;
        }

        if (keyStates.at('D')) {
            xTranslation -= perUnitDistance;
        }

        if (keyStates.at(VK_SPACE)) {
            yTranslation -= perUnitDistance;
        }

        if (keyStates.at(VK_SHIFT)) {
            yTranslation += perUnitDistance;
        }

        if (xAngle + m_xzPlaneAngle > DirectX::XM_PIDIV2) {
            xAngle = DirectX::XM_PIDIV2 - m_xzPlaneAngle;
        }
        else if (xAngle + m_xzPlaneAngle < -DirectX::XM_PIDIV2) {
            xAngle = -DirectX::XM_PIDIV2 - m_xzPlaneAngle;
        }

        DirectX::XMMATRIX rotationMatrix1 = DirectX::XMMatrixRotationX(-m_xzPlaneAngle);
        DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslation(xTranslation, yTranslation, zTranslation);
        DirectX::XMMATRIX rotationMatrix2 = DirectX::XMMatrixRotationY(yAngle);
        DirectX::XMMATRIX rotationMatrix3 = DirectX::XMMatrixRotationX(xAngle + m_xzPlaneAngle);

        m_viewMatrix = DirectX::XMMatrixMultiply(m_viewMatrix, rotationMatrix1);
        m_viewMatrix = DirectX::XMMatrixMultiply(m_viewMatrix, translationMatrix);
        m_viewMatrix = DirectX::XMMatrixMultiply(m_viewMatrix, rotationMatrix2);
        m_viewMatrix = DirectX::XMMatrixMultiply(m_viewMatrix, rotationMatrix3);

        m_xzPlaneAngle += xAngle;
    }

    void inline SetRotationSpeed(FLOAT rotationSpeed) { m_rotationSpeed = rotationSpeed; }
    void inline SetMovementSpeed(FLOAT movementSpeed) { m_movementSpeed = movementSpeed; }

    [[nodiscard]] inline DirectX::XMMATRIX GetViewMatrix() const { return m_viewMatrix; }
    [[nodiscard]] inline DirectX::XMFLOAT3 GetViewDirection() const
    {
        DirectX::XMVECTOR viewDirection =
            DirectX::XMVector4Transform(DirectX::g_XMIdentityR2, DirectX::XMMatrixInverse(nullptr, m_viewMatrix));
        DirectX::XMFLOAT3 viewDirectionFloat;
        DirectX::XMStoreFloat3(&viewDirectionFloat, viewDirection);

        return viewDirectionFloat;
    }

private:
    DirectX::XMMATRIX m_viewMatrix;
    FLOAT m_xzPlaneAngle;
    FLOAT m_rotationSpeed;
    FLOAT m_movementSpeed;
};
