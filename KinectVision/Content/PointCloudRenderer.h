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
            , renderPointCloudMesh(true)
        {
            CreateDeviceDependentResources();
            CreateWindowSizeDependentResources();
        }

        virtual ~PointCloudRenderer()
        {
            ReleaseDeviceDependentResources();
        }

        void EnableMeshRendering(bool meshRendering)
        {
            this->renderPointCloudMesh = meshRendering;
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
                    { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
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
                    { XMFLOAT3(-50.f, -50.f, -50.f), XMFLOAT3(-1.0f, -1.0f, -1.0f) },
                    { XMFLOAT3(-50.f, -50.f, 50.f), XMFLOAT3(-1.0f, -1.0f, 1.0f) },
                    { XMFLOAT3(-50.f, 50.f, -50.f), XMFLOAT3(-1.0f, 1.0f, -1.0f) },
                    { XMFLOAT3(-50.f, 50.f, 50.f), XMFLOAT3(-1.0f, 1.0f, 1.0f) },
                    { XMFLOAT3(50.f, -50.f, -50.f), XMFLOAT3(1.0f, -1.0f, -1.0f) },
                    { XMFLOAT3(50.f, -50.f, 50.f), XMFLOAT3(1.0f, -1.0f, 1.0f) },
                    { XMFLOAT3(50.f, 50.f, -50.f), XMFLOAT3(1.0f, 1.0f, -1.0f) },
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

                this->vertexCount = ARRAYSIZE(cubeVertices);

                // Load mesh indices. Each trio of indices represents
                // a triangle to be rendered on the screen.
                // For example: 0,2,1 means that the vertices with indexes
                // 0, 2 and 1 from the vertex buffer compose the 
                // first triangle of this mesh.
                static const unsigned int cubeIndices[] =
                {
                    0, 2, 1, // -x
                    1, 2, 3,

                    4, 5, 6, // +x
                    5, 7, 6,

                    0, 1, 5, // -y
                    0, 5, 4,

                    2, 6, 7, // +y
                    2, 7, 3,

                    0, 4, 6, // -z
                    0, 6, 2,

                    1, 3, 7, // +z
                    1, 7, 5,
                };

                this->indexCount = ARRAYSIZE(cubeIndices);

                D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
                indexBufferData.pSysMem = cubeIndices;
                indexBufferData.SysMemPitch = 0;
                indexBufferData.SysMemSlicePitch = 0;
                CD3D11_BUFFER_DESC indexBufferDesc(sizeof(cubeIndices), D3D11_BIND_INDEX_BUFFER);
                DX::ThrowIfFailed(
                    deviceResources->GetD3DDevice()->CreateBuffer(
                        &indexBufferDesc,
                        &indexBufferData,
                        &indexBuffer
                    )
                );
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
            float fovAngleY = 2.0f * 20.0f * XM_PI / 180.0f;

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

            // Setup our lighting parameters
            XMFLOAT4 vLightDirs[2] =
            {
                XMFLOAT4(-0.577f, 0.577f, -0.577f, 1.0f),
                XMFLOAT4(0.0f, .2f, 0.2f, 1.0f),
            };
            XMFLOAT4 vLightColors[2] =
            {
                XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
                XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f)
            };
            constantBufferData.vLightColor[0] = vLightColors[0];
            constantBufferData.vLightColor[1] = vLightColors[1];
            constantBufferData.vLightDir[0] = vLightDirs[0];
            constantBufferData.vLightDir[1] = vLightDirs[1];
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
            frame->ForEachInterPixel(ref new KinectVisionLib::InterPixelOp([this, cubeVertices, &verticesCount, frame](int x, int y, uint16 depth, uint16 depth1, uint16 depth2, uint16 depth3){
                const int centerX = frame->Width / 2;
                const int centerY = frame->Height / 2;
                const float d0 = 500.f;
                const float factor = 2.f;
                if (this->renderPointCloudMesh || depth != 0)
                {
                    cubeVertices[verticesCount].pos = XMFLOAT3((centerX - x) * depth / d0 / factor, (centerY - y) * depth / d0 / factor, depth / factor);

                    if (depth1 > 0 && depth2 > 0 && depth3 > 0)
                    {
                        XMFLOAT4 v1(0.f, 1.f, (float)(depth1 - depth) / 50.f, 1.f);
                        XMFLOAT4 v2(1.f, 0.f, (float)(depth2 - depth) / 50.f, 1.f);
                        XMStoreFloat3(&cubeVertices[verticesCount].normal, XMVector3Cross(XMLoadFloat4(&v1), XMLoadFloat4(&v2)));
                    }

                    verticesCount++;
                }
            }));

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

            if (this->renderPointCloudMesh)
            {
                CreatePointCloudIndices(frame, indexBuffer, &this->indexCount);
            }

            delete cubeVertices;
        }

        int GetIndex(int x, int y, int width)
        {
            return y * width + x;
        }

        void CreatePointCloudIndices(KinectVisionLib::Frame^ frame, Microsoft::WRL::ComPtr<ID3D11Buffer>&indexBuffer, int* indexCount)
        {
            int width = frame->Width - 1;
            int height = frame->Height;
            int countOfTriangle = (width - 1) * (height - 1) * 2;
            unsigned int* indices = new unsigned int[countOfTriangle * 3];

            int indexIndex = 0;
            frame->ForEachInterPixel(ref new KinectVisionLib::InterPixelOp([this, indices, &indexIndex, width](int x, int y, uint16 depth0, uint16 depth1, uint16 depth2, uint16 depth3){
                const int rangeThreshold = 50;
                if (depth0 > 200 && depth1 > 200 && depth2 > 200 && abs(depth0 - depth1) < rangeThreshold && abs(depth0 - depth2) < rangeThreshold)
                {
                    indices[indexIndex] = GetIndex(x, y, width);
                    indices[indexIndex + 1] = GetIndex(x + 1, y, width);
                    indices[indexIndex + 2] = GetIndex(x, y + 1, width);
                    indexIndex += 3;
                }
                if (depth1 > 200 && depth2 > 200 && depth3 > 200 && abs(depth1 - depth2) < rangeThreshold && abs(depth1 - depth3) < rangeThreshold)
                {
                    indices[indexIndex] = GetIndex(x + 1, y, width);
                    indices[indexIndex + 1] = GetIndex(x + 1, y + 1, width);
                    indices[indexIndex + 2] = GetIndex(x, y + 1, width);
                    indexIndex += 3;
                }
            }));
            if (indexCount != nullptr)
            {
                *indexCount = indexIndex;
            }

            D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
            indexBufferData.pSysMem = indices;
            indexBufferData.SysMemPitch = 0;
            indexBufferData.SysMemSlicePitch = 0;
            CD3D11_BUFFER_DESC indexBufferDesc(sizeof(*indices) * indexIndex, D3D11_BIND_INDEX_BUFFER);
            DX::ThrowIfFailed(
                this->deviceResources->GetD3DDevice()->CreateBuffer(
                &indexBufferDesc,
                &indexBufferData,
                &indexBuffer
                )
            );
            delete indices;
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

            RenderPointCloud(context);

            // Draw coordiante axes
            RenderCoordinateAxes(context);
        }

        void RenderCoordinateAxes(ID3D11DeviceContext2* context)
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

        void RenderPointCloud(ID3D11DeviceContext2* context)
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

            if (this->renderPointCloudMesh)
            {
                context->IASetIndexBuffer(
                    this->indexBuffer.Get(),
                    DXGI_FORMAT_R32_UINT,
                    0);

                context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            }
            else
            {
                context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
            }

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

            // Send the constant buffer to the graphics device.
            context->PSSetConstantBuffers(
                0,
                1,
                constantBuffer.GetAddressOf()
            );

            // Draw the objects.
            if (this->renderPointCloudMesh)
            {
                context->DrawIndexed(this->indexCount, 0, 0);
            }
            else
            {
                context->Draw(this->vertexCount, 0);
            }
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
        int indexCount;
        float startPosition;

        bool renderPointCloudMesh;      // Should we render as mesh or render as points

        ModelViewProjectionConstantBuffer    constantBufferData;

        // Direct3D resources for cube geometry.
        Microsoft::WRL::ComPtr<ID3D11InputLayout>    inputLayout;
        Microsoft::WRL::ComPtr<ID3D11Buffer>        vertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer>        indexBuffer;
        Microsoft::WRL::ComPtr<ID3D11VertexShader>    vertexShader;
        Microsoft::WRL::ComPtr<ID3D11PixelShader>    pixelShader;
        Microsoft::WRL::ComPtr<ID3D11Buffer>        constantBuffer;

        Microsoft::WRL::ComPtr<ID3D11Buffer> coordinateAxewsVertexBuffer;
    };
}
