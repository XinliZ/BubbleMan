#pragma once

#include "FrameMask.h"
#include "DeltaMotionState.h"
#include "../DepthImage.h"
#include "ErrorMap.h"

namespace KinectVisionLib
{
    namespace Core
    {
        class MotionState;

        class Chip
        {
        public:
            Chip(shared_ptr<const DepthImage> depthImage, shared_ptr<const AreaMap> areaMap, uint16 areaCode)
                : mask(areaMap, areaCode)
                , center(0, 0, 0)
                , frame(depthImage)
            {
            }

            //Chip* FindMatch(Frame* frame, MotionState* motionState, float64* score);
            shared_ptr<const ErrorMap> Match(shared_ptr<const DepthImage> depthFrame, DeltaMotionState& deltaMotionState)
            {
                shared_ptr<ErrorMap> errorMap = make_shared<ErrorMap>(mask.GetWidth(), mask.GetHeight());
                const int centerX = frame->GetWidth() / 2;
                const int centerY = frame->GetHeight() / 2;
                for (int i = 0; i < frame->GetHeight(); i++)
                {
                    const uint16* scan = frame->GetScan0() + i * frame->GetStride();
                    for (int j = 0; j < frame->GetWidth(); j++)
                    {
                        if (mask.IsPixelValid(Point(j, i)))
                        {
                            DepthPixel outPixel = deltaMotionState.TransformDepthPixel(center, DepthPixel(j - centerX, i - centerY, *scan));
                            auto pixel = depthFrame->GetPixelSafe(Point(outPixel.GetX(), outPixel.GetY()));
                            auto error = pixel - outPixel.GetDepth();
                        }
                    }
                }
                return errorMap;
            }

        private:
            shared_ptr<const DepthImage> frame;
            FrameMask mask;
            DepthPixel center;

            DeltaMotionState offset;
            DeltaMotionState totalOffset;
        };
    }
}
