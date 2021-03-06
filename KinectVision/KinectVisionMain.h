﻿#pragma once

#include "Common\StepTimer.h"
#include "Common\DeviceResources.h"
#include "Content\SampleFpsTextRenderer.h"
#include "Content\PointCloudRenderer.h"

// Renders Direct2D and 3D content on the screen.
namespace KinectVision
{
    ref class KinectManager;

    class KinectVisionMain : public DX::IDeviceNotify
    {
    public:
        KinectVisionMain(const std::shared_ptr<DX::DeviceResources>& deviceResources);
        ~KinectVisionMain();
        void CreateWindowSizeDependentResources();
        void StartTracking() { m_sceneRenderer->StartTracking(); }
        void TrackingUpdate(float positionX) { m_pointerLocationX = positionX; }
        void StopTracking() { m_sceneRenderer->StopTracking(); }
        bool IsTracking() { return m_sceneRenderer->IsTracking(); }
        void StartRenderLoop(KinectManager^ kinectManager);
        void StopRenderLoop();
        void EnableMeshRendering(bool enabled) { m_sceneRenderer->EnableMeshRendering(enabled); }
        Concurrency::critical_section& GetCriticalSection() { return m_criticalSection; }

        // IDeviceNotify
        virtual void OnDeviceLost();
        virtual void OnDeviceRestored();

    private:
        void ProcessInput(KinectManager^ kinectManager);
        void Update(KinectManager^ kinectManager);
        bool Render();

        // Cached pointer to device resources.
        std::shared_ptr<DX::DeviceResources> m_deviceResources;

        // TODO: Replace with your own content renderers.
        std::unique_ptr<PointCloudRenderer> m_sceneRenderer;
        std::unique_ptr<SampleFpsTextRenderer> m_fpsTextRenderer;

        Windows::Foundation::IAsyncAction^ m_renderLoopWorker;
        Concurrency::critical_section m_criticalSection;

        // Rendering loop timer.
        DX::StepTimer m_timer;

        // Track current input pointer position.
        float m_pointerLocationX;
    };
}