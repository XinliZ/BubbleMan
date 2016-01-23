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
            Chip()
                : mask(Rect(0, 0, 0, 0))        // TODO: Fix this
                , center(0, 0, 0)
            {}

            //Chip* FindMatch(Frame* frame, MotionState* motionState, float64* score);
            shared_ptr<ErrorMap> Match(shared_ptr<DepthImage> depthFrame, DeltaMotionState& deltaMotionState)
            {
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
            }

        private:
            shared_ptr<DepthImage> frame;
            FrameMask mask;
            DepthPixel center;

            DeltaMotionState offset;
            DeltaMotionState totalOffset;
        };
    }
}
