#include "pch.h"
#include "Frame.h"
#include "Core/DepthImage.h"

using namespace KinectVisionLib;
using namespace Microsoft::Graphics::Canvas;
using namespace Platform;

Frame::Frame()
{
}
//
//Frame::Frame(int width, int height, const Platform::Array<byte>^ bitmapBuffer)
//{
//    if (bitmapBuffer->Length != width * height)
//    {
//        throw ref new InvalidArgumentException("Invalid input. The buffer size doesn't match the pixel count");
//    }
//
//    this->image = std::make_shared<KinectVisionLib::Core::DepthImage>(width, height, bitmapBuffer);
//}

Frame::Frame(WindowsPreview::Kinect::DepthFrame^ depthFrame)
{
    // TODO: To be impelemented
}

Frame::Frame(std::shared_ptr<KinectVisionLib::Core::Image<uint16>> image)
{
    this->image = image;
}

Frame::Frame(const Platform::Array<uint16>^ bitmapBuffer, int width, int height)
{
    if (bitmapBuffer->Length != width * height)
    {
        throw ref new InvalidArgumentException("Invalid input. The buffer size doesn't match the pixel count");
    }

    this->image = std::make_shared<KinectVisionLib::Core::DepthImage>(bitmapBuffer->Data, width, height);
}

CanvasBitmap^ Frame::GetBitmap(ICanvasResourceCreator^ canvas)
{
    if (image != nullptr)
    {
        return CreateBitmapFromBuffer(canvas, this->image);
    }
    return nullptr;
}
//
//void Frame::CopyToArray(Platform::WriteOnlyArray<uint16>^ buffer)
//{
//    if (buffer->Length < this->bitmapBuffer16->Length)
//    {
//        throw ref new InvalidArgumentException("The input buffer is insufficent to copy the frame data.");
//    }
//
//    for (unsigned int i = 0; i < buffer->Length; i++)
//    {
//        buffer->Data[i] = this->bitmapBuffer16[i];
//    }
//}

CanvasBitmap^ Frame::CreateBitmapFromBuffer(ICanvasResourceCreator^ canvas, std::shared_ptr<KinectVisionLib::Core::Image<uint16>> image)
{
    // RGBA buffer
    auto bitmapBuffer = ref new Array<byte>(image->GetWidth() * image->GetHeight() * 4);

    image->ToDisplay(bitmapBuffer->Data);

    return CanvasBitmap::CreateFromBytes(canvas, bitmapBuffer, image->GetWidth(), image->GetHeight(), Microsoft::Graphics::Canvas::DirectX::DirectXPixelFormat::R8G8B8A8UIntNormalized);

}
