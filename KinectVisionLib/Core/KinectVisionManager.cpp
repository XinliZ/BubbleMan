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

shared_ptr<const ErrorMap> KinectVisionManager::TransformFrame(shared_ptr<const DepthImage> image, shared_ptr<const DepthImage> previousImage, float dX, float dY, float dZ, float dA, float dB, float dR)
{
    auto target = make_shared<ErrorMap>(image->GetSize());
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

            float xTarget = x1 + center.GetX();
            float yTarget = center.GetY() - y1;

            uint16 previousDepth;
            if (previousImage->GetPixelInterpolation(xTarget, yTarget, previousDepth))
            {
                target->SetPixel(Point(xIn, yIn), (int16)(depth1 - previousDepth));
            }
            else
            {
                target->SetPixelInvalid(Point(xIn, yIn));
            }
        }
        else
        {
            target->SetPixelInvalid(Point(xIn, yIn));
        }
    });
    return target;
}