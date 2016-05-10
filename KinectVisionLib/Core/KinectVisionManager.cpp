#include "pch.h"
#include "KinectVisionManager.h"

using namespace KinectVisionLib::Core;

KinectVisionManager::KinectVisionManager()
{
}


KinectVisionManager::~KinectVisionManager()
{
}

void KinectVisionManager::UpdateBlackDotsMask(shared_ptr<DepthImage> image, shared_ptr<DepthImage> newImage)
{
    for (int i = 0; i < image->GetHeight(); i++)
    {
        uint16* target = image->GetScan0() + i * image->GetStride();
        uint16* source = newImage->GetScan0() + i * newImage->GetStride();
        for (int j = 0; j < image->GetWidth(); j++)
        {
            *target += (*source == 0) ? 15 : 0;
            if (*target > 0) *target--;

            target++;
            source++;
        }
    }
}

shared_ptr<const DepthImage> KinectVisionManager::TransformFrame(shared_ptr<const DepthImage> image, float dX, float dY, float dZ, float dA, float dB, float dR)
{
    auto target = make_shared<DepthImage>(image->GetSize());
    Point center = image->GetCenter();
    image->ImageOperation([=](int xIn, int yIn, uint16 depth) {
        if (depth > 0)
        {
            // Find the transformed pixel
            // TODO: What is the order?

            // x/y is the depth image plane (origin in the middle and x/y points to right/up). z is the depth (point to the scene from camera). 
            // dA is the rotation around axis z, and dB is the rotation around axis x and dR is around y. Left hand rotation.
            float x = (float)(xIn - center.GetX());
            float y = (float)(center.GetY() - yIn);

            float depth1 = depth + dZ;
            float x1 = x * depth1 / depth + dX / depth;
            float y1 = y * depth1 / depth + dY / depth;

            Point newPoint((int)(x1 + 0.5f) + center.GetX(), center.GetY() - (int)(y1 + 0.5f));
            if (target->GetRect().Contains(newPoint))
            {
                target->SetPixel(newPoint, (uint16)depth1);
            }
        }
    });
    return target;
}