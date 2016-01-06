#pragma once

#include "ShaderStructures.h"
#include "..\Common\StepTimer.h"
#include "..\Common\DeviceResources.h"
#include "..\Common\DirectXHelper.h"
#include "..\KinectManager.h"

namespace KinectVision
{
    using namespace DirectX;
    using namespace Windows::Foundation;

    class PointCloudRenderer
    {
    public:

        PointCloudRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources)
            : deviceResources(deviceResources)
            , startPosition(XM_PI + 0.1f)
            , vertexCount(0)
            , isTracking(false)
            , loadingComplete(false)
        {
            CreateDeviceDependentResources();
            CreateWindowSizeDependentResources();
        }

        virtual ~PointCloudRenderer()
        {
            ReleaseDeviceDependentResources();
        }

        void CreateDeviceDependentResources()
        {
            // Load shaders asynchronously.
            auto loadVSTask = DX::ReadDataAsync(L"SampleVertexShader.cso");
            auto loadPSTask = DX::ReadDataAsync(L"SamplePixelShader.cso");

            // After the vertex shader file is loaded, create the shader and input layout.
            auto createVSTask = loadVSTask.then([this](const std::vector<byte>& fileData) {
                DX::ThrowIfFailed(
                    deviceResources->GetD3DDevice()->CreateVertexShader(
                    &fileData[0],
                    fileData.size(),
                    nullptr,
                    &vertexShader
                    )
                );

                static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
                {
                    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
                    { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
                };

                DX::ThrowIfFailed(
                    deviceResources->GetD3DDevice()->CreateInputLayout(
                    vertexDesc,
                    ARRAYSIZE(vertexDesc),
                    &fileData[0],
                    fileData.size(),
                    &inputLayout
                    )
                );
            });

            // After the pixel shader file is loaded, create the shader and constant buffer.
            auto createPSTask = loadPSTask.then([this](const std::vector<byte>& fileData) {
                DX::ThrowIfFailed(
                    deviceResources->GetD3DDevice()->CreatePixelShader(
                    &fileData[0],
                    fileData.size(),
                    nullptr,
                    &pixelShader
                    )
                );

                CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
                DX::ThrowIfFailed(
                    deviceResources->GetD3DDevice()->CreateBuffer(
                    &constantBufferDesc,
                    nullptr,
                    &constantBuffer
                    )
                );
            });

            // Once both shaders are loaded, create the mesh.
            auto createCubeTask = (createPSTask && createVSTask).then([this]() {

                // Load mesh vertices. Each vertex has a position and a color.
                static const VertexPositionColor cubeVertices[] =
                {
                    { XMFLOAT3(-50.f, -50.f, -50.f), XMFLOAT3(1.0f, 1.0f, 1.0f) },
                    { XMFLOAT3(-50.f, -50.f, 50.f), XMFLOAT3(1.0f, 1.0f, 1.0f) },
                    { XMFLOAT3(-50.f, 50.f, -50.f), XMFLOAT3(1.0f, 1.0f, 1.0f) },
                    { XMFLOAT3(-50.f, 50.f, 50.f), XMFLOAT3(1.0f, 1.0f, 1.0f) },
                    { XMFLOAT3(50.f, -50.f, -50.f), XMFLOAT3(1.0f, 1.0f, 1.0f) },
                    { XMFLOAT3(50.f, -50.f, 50.f), XMFLOAT3(1.0f, 1.0f, 1.0f) },
                    { XMFLOAT3(50.f, 50.f, -50.f), XMFLOAT3(1.0f, 1.0f, 1.0f) },
                    { XMFLOAT3(50.f, 50.f, 50.f), XMFLOAT3(1.0f, 1.0f, 1.0f) },
                };

                D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
                vertexBufferData.pSysMem = cubeVertices;
                vertexBufferData.SysMemPitch = 0;
                vertexBufferData.SysMemSlicePitch = 0;
                CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(cubeVertices), D3D11_BIND_VERTEX_BUFFER);
                DX::ThrowIfFailed(
                    deviceResources->GetD3DDevice()->CreateBuffer(
                    &vertexBufferDesc,
                    &vertexBufferData,
                    &this->vertexBuffer
                    )
                );

                vertexCount = ARRAYSIZE(cubeVertices);
            });

            auto createCoordinateAxesTask = (createPSTask && createVSTask).then([this](){
                // Load mesh vertices. Each vertex has a position and a color.
                static const VertexPositionColor cubeVertices[] =
                {
                    { XMFLOAT3(-100.f, 0.f, 0.f), XMFLOAT3(0.5f, 0.0f, 0.0f) },
                    { XMFLOAT3(100.f, 0.f, 0.f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
                    { XMFLOAT3(0.f, -100.f, 0.f), XMFLOAT3(0.0f, 0.5f, 0.0f) },
                    { XMFLOAT3(0.f, 100.f, 0.f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
                    { XMFLOAT3(0.f, 0.f, -100.f), XMFLOAT3(0.4f, 0.4f, 0.5f) },
                    { XMFLOAT3(0.f, 0.f, 100.f), XMFLOAT3(0.4f, 0.4f, 1.0f) },
                };

                D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
                vertexBufferData.pSysMem = cubeVertices;
                vertexBufferData.SysMemPitch = 0;
                vertexBufferData.SysMemSlicePitch = 0;
                CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(cubeVertices), D3D11_BIND_VERTEX_BUFFER);
                DX::ThrowIfFailed(
                    deviceResources->GetD3DDevice()->CreateBuffer(
                    &vertexBufferDesc,
                    &vertexBufferData,
                    &this->coordinateAxewsVertexBuffer
                    )
                );
            });

            // Once the cube is loaded, the object is ready to be rendered.
            (createCubeTask && createCoordinateAxesTask).then([this]() {
                loadingComplete = true;
            });
        }

        void CreateWindowSizeDependentResources()
        {
            Size outputSize = deviceResources->GetOutputSize();
            float aspectRatio = outputSize.Width / outputSize.Height;
            float fovAngleY = 40.0f * XM_PI / 180.0f;

            // Note that the OrientationTransform3D matrix is post-multiplied here
            // in order to correctly orient the scene to match the display orientation.
            // This post-multiplication step is required for any draw calls that are
            // made to the swap chain render target. For draw calls to other targets,
            // this transform should not be applied.

            // This sample makes use of a right-handed coordinate system using row-major matrices.
            XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovRH(
                fovAngleY,
                aspectRatio,
                0.01f,
                10000.0f
                );

            XMFLOAT4X4 orientation = deviceResources->GetOrientationTransform3D();

            XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);

            XMStoreFloat4x4(
                &constantBufferData.projection,
                XMMatrixTranspose(perspectiveMatrix * orientationMatrix)
                );

            // Eye is at (0,0.7,1.5), looking at point (0,-0.1,0) with the up-vector along the y-axis.
            static const XMVECTORF32 eye = { 0.0f, -50.f, 400.5f, 0.0f };
            static const XMVECTORF32 at = { 0.0f, -20.f, 0.0f, 0.0f };
            static const XMVECTORF32 up = { 0.0f, 0.0f, 50.0f, 0.0f };

            XMStoreFloat4x4(&constantBufferData.view, XMMatrixTranspose(XMMatrixLookAtRH(eye, at, up)));

            Rotate(startPosition);
        }

        void ReleaseDeviceDependentResources()
        {
            loadingComplete = false;
            vertexShader.Reset();
            inputLayout.Reset();
            pixelShader.Reset();
            constantBuffer.Reset();
            vertexBuffer.Reset();
            coordinateAxewsVertexBuffer.Reset();
        }

        void UpdateDepthFrame(KinectManager^ kinectManager)
        {
            auto frame = kinectManager->GetDepthFrame();
            if (frame == nullptr)
                return;

            static KinectVisionLib::Frame^ oldFrame = nullptr;
            if (frame == oldFrame)
            {
                return;
            }
            oldFrame = frame;

            // Load mesh vertices. Each vertex has a position and a color.
            VertexPositionColor* cubeVertices = new VertexPositionColor[frame->Width * frame->Height];
            int verticesCount = 0;
            frame->ForEachPixel(ref new KinectVisionLib::PixelOp([cubeVertices, &verticesCount, frame](int x, int y, uint16 depth){
                const int midX = frame->Width / 2;
                const int midY = frame->Height / 2;
                const float d0 = 500.f;
                const float factor = 2.f;
                if (depth != 0)
                {
                    cubeVertices[verticesCount].color = XMFLOAT3(1.0f, 1.0f, 1.0f);
                    cubeVertices[verticesCount].pos = XMFLOAT3((x - midX) * depth / d0 / factor, (midY - y) * depth / d0 / factor, depth / factor);
                    verticesCount++;
                }
            }));

            //this->vertexBuffer.Reset();

            D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
            vertexBufferData.pSysMem = cubeVertices;
            vertexBufferData.SysMemPitch = 0;
            vertexBufferData.SysMemSlicePitch = 0;
            CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(VertexPositionColor) * verticesCount, D3D11_BIND_VERTEX_BUFFER);
            DX::ThrowIfFailed(
                deviceResources->GetD3DDevice()->CreateBuffer(
                    &vertexBufferDesc,
                    &vertexBufferData,
                    &this->vertexBuffer
                )
            );

            this->vertexCount = verticesCount;
            delete cubeVertices;
        }

        void Update(DX::StepTimer const& timer)
        {

        }

        void Render()
        {
            // Loading is asynchronous. Only draw geometry after it's loaded.
            if (!loadingComplete)
            {
                return;
            }

            auto context = deviceResources->GetD3DDeviceContext();

            DrawCube(context);

            // Draw coordiante axes
            DrawCoordinateAxes(context);
        }

        void DrawCoordinateAxes(ID3D11DeviceContext2* context)
        {
            // Prepare the constant buffer to send it to the graphics device.
            context->UpdateSubresource(
                constantBuffer.Get(),
                0,
                NULL,
                &constantBufferData,
                0,
                0
            );

            // Each vertex is one instance of the VertexPositionColor struct.
            UINT stride = sizeof(VertexPositionColor);
            UINT offset = 0;
            context->IASetVertexBuffers(
                0,
                1,
                this->coordinateAxewsVertexBuffer.GetAddressOf(),
                &stride,
                &offset
            );

            context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

            context->IASetInputLayout(inputLayout.Get());

            // Attach our vertex shader.
            context->VSSetShader(
                vertexShader.Get(),
                nullptr,
                0
            );

            // Send the constant buffer to the graphics device.
            context->VSSetConstantBuffers(
                0,
                1,
                constantBuffer.GetAddressOf()
            );

            // Attach our pixel shader.
            context->PSSetShader(
                pixelShader.Get(),
                nullptr,
                0
            );

            // Draw the objects.
            context->Draw(6, 0);
        }

        void DrawCube(ID3D11DeviceContext2* context)
        {
            // Prepare the constant buffer to send it to the graphics device.
            context->UpdateSubresource(
                constantBuffer.Get(),
                0,
                NULL,
                &constantBufferData,
                0,
                0
            );

            // Each vertex is one instance of the VertexPositionColor struct.
            UINT stride = sizeof(VertexPositionColor);
            UINT offset = 0;
            context->IASetVertexBuffers(
                0,
                1,
                vertexBuffer.GetAddressOf(),
                &stride,
                &offset
            );

            context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

            context->IASetInputLayout(inputLayout.Get());

            // Attach our vertex shader.
            context->VSSetShader(
                vertexShader.Get(),
                nullptr,
                0
            );

            // Send the constant buffer to the graphics device.
            context->VSSetConstantBuffers(
                0,
                1,
                constantBuffer.GetAddressOf()
            );

            // Attach our pixel shader.
            context->PSSetShader(
                pixelShader.Get(),
                nullptr,
                0
            );

            // Draw the objects.
            context->Draw(this->vertexCount, 0);
        }

#pragma region Tracking_code
        void StartTracking()
        {
            isTracking = true;
        }

        void StopTracking()
        {
            isTracking = false;
        }

        bool IsTracking()
        {
            return isTracking;
        }

        void TrackingUpdate(float positionX)
        {
            if (isTracking)
            {
                float radians = startPosition + XM_2PI * 2.0f * positionX / deviceResources->GetOutputSize().Width;
                Rotate(radians);
            }
        }
#pragma endregion Tracking_code

    private:
        void Rotate(float radians)
        {
            XMStoreFloat4x4(&constantBufferData.model, XMMatrixTranspose(XMMatrixRotationY(radians)));
        }

    private:
        std::shared_ptr<DX::DeviceResources> deviceResources;
        bool isTracking;
        bool loadingComplete;
        int vertexCount;
        float startPosition;

        ModelViewProjectionConstantBuffer    constantBufferData;

        // Direct3D resources for cube geometry.
        Microsoft::WRL::ComPtr<ID3D11InputLayout>    inputLayout;
        Microsoft::WRL::ComPtr<ID3D11Buffer>        vertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11VertexShader>    vertexShader;
        Microsoft::WRL::ComPtr<ID3D11PixelShader>    pixelShader;
        Microsoft::WRL::ComPtr<ID3D11Buffer>        constantBuffer;

        Microsoft::WRL::ComPtr<ID3D11Buffer> coordinateAxewsVertexBuffer;
    };
}
