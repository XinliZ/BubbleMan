#pragma once

#include "FrameMask.h"
#include "DeltaMotionState.h"
#include "../DepthImage.h"
#include "../ImageBuffer.h"
#include "ErrorMap.h"

namespace KinectVisionLib
{
    namespace Core
    {
        class MotionState;

        class Chip
        {
        public:
            Chip(shared_ptr<const DepthImage> depthFrame, shared_ptr<const AreaMap> areaMap, uint16 areaCode)
                : mask(areaMap, areaCode)
                , center(0, 0, 0)
                , frame(depthFrame)
            {
            }

            //Chip* FindMatch(Frame* frame, MotionState* motionState, float64* score);
            shared_ptr<const ErrorMap> Match(shared_ptr<const DepthImage> depthFrame, DeltaMotionState& deltaMotionState)
            {
                const int centerX = frame->GetWidth() / 2;
                const int centerY = frame->GetHeight() / 2;

                ImageBuffer<uint16> targetImage(depthFrame->GetSize());
                mask.ImageOperation<uint16, uint16>(frame.get(), &targetImage, [centerX, centerY, &deltaMotionState, this](const bool* maskScan, const uint16* originScan, int x, int y, ImageBuffer<uint16>* target) {
                    if (*maskScan)
                    {
                        DepthPixel outPixel = deltaMotionState.TransformDepthPixel(this->center, DepthPixel(x - centerX, y - centerY, *originScan));
                        auto pixel = target->GetPixel(Point(outPixel.GetX(), outPixel.GetY()));
                        if (pixel == 0 || pixel < outPixel.GetDepth())
                        {
                            target->SetPixel(Point(outPixel.GetX(), outPixel.GetY()), outPixel.GetDepth());
                        }
                    }
                });

                shared_ptr<ErrorMap> errorMap = make_shared<ErrorMap>(depthFrame->GetSize());
                depthFrame->ImageOperation<uint16, int16>(&targetImage, errorMap.get(), [](const uint16* inputScan, const uint16* resultScan, int16* errorScan) {
                    *errorScan = *inputScan - *resultScan;
                });
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
