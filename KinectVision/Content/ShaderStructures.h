#pragma once

namespace KinectVision
{
    // Constant buffer used to send MVP matrices to the vertex shader.
    struct ModelViewProjectionConstantBuffer
    {
        DirectX::XMFLOAT4X4 model;
        DirectX::XMFLOAT4X4 view;
        DirectX::XMFLOAT4X4 projection;
        DirectX::XMFLOAT4 vLightDir[2];
        DirectX::XMFLOAT4 vLightColor[2];
    };

    // Used to send per-vertex data to the vertex shader.
    struct VertexPositionColor
    {
        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT3 normal;
    };
}