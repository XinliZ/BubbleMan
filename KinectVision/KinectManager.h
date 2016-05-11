#pragma once

#include "ThreadSafeBox.h"

namespace KinectVision
{
    ref class KinectManager sealed
    {
    public:
        KinectManager();
        void Initialize(Microsoft::Graphics::Canvas::ICanvasResourceCreator^ canvas);

        void ConnectToKinect();
        void DisconnectKinect();

        void LoadFolder();
        void LoadNextFrame();

        void RenderView(Microsoft::Graphics::Canvas::CanvasDrawingSession^ drawingSession);
        KinectVisionLib::Frame^ GetDepthFrame() { return this->currentFrame; }
        KinectVisionLib::Frame^ GetPreviousDepthFrame() { return this->previousFrame; }

        // Image processing part
        bool ProcessImage(float dX, float dY, float dZ, float dA, float dB, float dR);

    private:
        WindowsPreview::Kinect::KinectSensor^ kinectSensor;
        WindowsPreview::Kinect::MultiSourceFrameReader^ multiSourceFrameReader;

        Microsoft::Graphics::Canvas::ICanvasResourceCreator^ canvasResourceCreator;
        ThreadSafeBox<Microsoft::Graphics::Canvas::CanvasBitmap^> canvasBitmap0;
        ThreadSafeBox<Microsoft::Graphics::Canvas::CanvasBitmap^> canvasBitmap1;
        KinectVisionLib::Frame^ currentFrame;
        KinectVisionLib::Frame^ previousFrame;

        Windows::Foundation::Collections::IVectorView<Windows::Storage::StorageFile^>^ fileList;
        uint32 frameNumber;

        KinectVisionLib::KinectVision^ kinectVision;

    private:
        void OnMultiSourceFrameArrived(WindowsPreview::Kinect::MultiSourceFrameReader ^sender, WindowsPreview::Kinect::MultiSourceFrameArrivedEventArgs ^args);
        Microsoft::Graphics::Canvas::CanvasBitmap^ BuildCanvasBitmapFromDepthFrame(Microsoft::Graphics::Canvas::ICanvasResourceCreator^ canvas, WindowsPreview::Kinect::DepthFrame^ depthFrame);
        void NormalizeDepthDataToBuffer(Platform::Array<byte>^ bitmapBuffer, Platform::Array<uint16>^ sourceBuffer, uint16 minDepth, uint16 maxDepth);
        void ProcessFrame(KinectVisionLib::Frame^ bitmap);
        Microsoft::Graphics::Canvas::CanvasBitmap^ CreateBitmapFromBuffer(Microsoft::Graphics::Canvas::ICanvasResourceCreator^ canvas, Platform::Array<uint16>^ buffer, int width, int height, int minDepth, int maxDepth);
        Microsoft::Graphics::Canvas::CanvasBitmap^ BuildCanvasBitmapFromBitmap(Microsoft::Graphics::Canvas::ICanvasResourceCreator^ canvas, Microsoft::Graphics::Canvas::CanvasBitmap^ bitmap);

        KinectVisionLib::Frame^ KinectManager::BuildFrameFromBitmap(Microsoft::Graphics::Canvas::CanvasBitmap^ bitmap);
        KinectVisionLib::Frame^ KinectManager::BuildFrameFromDepthFrame(WindowsPreview::Kinect::DepthFrame^ depthFrame);

    };
}
