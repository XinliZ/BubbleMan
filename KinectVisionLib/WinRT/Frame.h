#pragma once

#include "Core/DepthImage.h"

namespace KinectVisionLib
{
    public delegate void PixelOp(int x, int y, uint16 depth);
    public delegate void InterPixelOp(int x, int y, uint16 depth0, uint16 depth1, uint16 depth2, uint16 depth3);

    public ref class Frame sealed
    {
    public:
        [Windows::Foundation::Metadata::DefaultOverloadAttribute]
        Frame();

        Frame(const Platform::Array<uint16>^ bitmapBuffer, int width, int height);


        Microsoft::Graphics::Canvas::CanvasBitmap^ GetBitmap(Microsoft::Graphics::Canvas::ICanvasResourceCreator^ canvas);
        void ForEachPixel(PixelOp^ action);
        void ForEachInterPixel(InterPixelOp^ action);

        property int Width {int get(){ return image->GetWidth(); }}
        property int Height {int get(){ return image->GetHeight(); }}
        property int Length {int get(){ return image->GetLength(); }}
        property int LengthInPixel {int get(){ return image->GetWidth() * image->GetHeight(); }}

    private:
        Microsoft::Graphics::Canvas::CanvasBitmap^ CreateBitmapFromBuffer(Microsoft::Graphics::Canvas::ICanvasResourceCreator^ canvas, std::shared_ptr<KinectVisionLib::Core::Image<uint16>> image);

    internal:
        Frame(std::shared_ptr<KinectVisionLib::Core::Image<uint16>> image);
        std::shared_ptr<KinectVisionLib::Core::DepthImage> GetImage() { return std::dynamic_pointer_cast<KinectVisionLib::Core::DepthImage>(this->image); }

    private:
        std::shared_ptr<KinectVisionLib::Core::Image<uint16>> image;
    };
}
