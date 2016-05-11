#include "pch.h"
#include "Frame.h"
#include "Core/DepthImage.h"

using namespace KinectVisionLib;
using namespace Microsoft::Graphics::Canvas;
using namespace Platform;

Frame::Frame()
{
}

Frame::Frame(std::shared_ptr<const KinectVisionLib::Core::ImageBase> image)
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

void Frame::ForEachPixel(PixelOp^ action)
{
    const KinectVisionLib::Core::DepthImage * img = dynamic_cast<const KinectVisionLib::Core::DepthImage*>(this->image.get());
    if (img != nullptr)
    {
        for (int i = 0; i < img->GetHeight(); i++)
        {
            const uint16* scan = img->GetScan0() + i * img->GetStride();
            for (int j = 0; j < img->GetWidth(); j++)
            {
                action(j, i, *scan);

                scan++;
            }
        }
    }
    else
    {
        throw new exception("Invalid image type");
    }
}

void Frame::ForEachInterPixel(InterPixelOp^ action)
{
    const KinectVisionLib::Core::DepthImage * img = dynamic_cast<const KinectVisionLib::Core::DepthImage*>(this->image.get());
    if (img != nullptr)
    {
        for (int i = 0; i < img->GetHeight() - 1; i++)
        {
            const uint16* scan = img->GetScan0() + i * img->GetStride();
            const uint16* scan1 = img->GetScan0() + (i + 1) * img->GetStride();
            for (int j = 0; j < img->GetWidth() - 1; j++)
            {
                action(j, i, *scan, *(scan + 1), *scan1, *(scan1 + 1));

                scan++;
                scan1++;
            }
        }
    }
    else
    {
        throw new exception("Invalid image type");
    }
}

CanvasBitmap^ Frame::CreateBitmapFromBuffer(ICanvasResourceCreator^ canvas, std::shared_ptr<const KinectVisionLib::Core::ImageBase> image)
{
    // RGBA buffer
    auto bitmapBuffer = ref new Array<byte>(image->GetWidth() * image->GetHeight() * 4);

    image->RenderAsRGBA(bitmapBuffer->Data);

    return CanvasBitmap::CreateFromBytes(canvas, bitmapBuffer, image->GetWidth(), image->GetHeight(), Microsoft::Graphics::Canvas::DirectX::DirectXPixelFormat::R8G8B8A8UIntNormalized);
}
