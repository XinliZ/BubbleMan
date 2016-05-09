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
            void Match(shared_ptr<const DepthImage> depthFrame, const DeltaMotionState& deltaMotionState, ErrorMap& errorMap) const
            {
                ImageBuffer<uint16> targetImage(depthFrame->GetSize());
                TransformImage(frame, &mask, deltaMotionState, &targetImage);

                depthFrame->ImageOperation<uint16, int16>(&targetImage, &errorMap, [](const uint16* inputScan, const uint16* transformedScan, int16* errorScan) {
                    if (*transformedScan != 0)
                    {
                        *errorScan = *inputScan - *transformedScan;
                    }
                });
            }

            void TransformImage(shared_ptr<const DepthImage> frame, const FrameMask* mask, const DeltaMotionState& deltaMotionState, ImageBuffer<uint16>* targetImage) const
            {
                const int centerX = frame->GetWidth() / 2;
                const int centerY = frame->GetHeight() / 2;

                mask->ImageOperation<uint16, uint16>(frame.get(), targetImage, [centerX, centerY, &deltaMotionState, this](const bool* maskScan, const uint16* originScan, int x, int y, ImageBuffer<uint16>* target) {
                    if (*maskScan)
                    {
                        DepthPixel outPixel = deltaMotionState.TransformDepthPixel(this->center, DepthPixel(x - centerX, y - centerY, *originScan));
                        if (target->GetRect().Contains(Point(outPixel.GetX(), outPixel.GetY())))
                        {
                            auto pixel = target->GetPixel(Point(outPixel.GetX(), outPixel.GetY()));
                            if (pixel == 0 || pixel < outPixel.GetDepth())
                            {
                                target->SetPixel(Point(outPixel.GetX(), outPixel.GetY()), outPixel.GetDepth());
                            }
                        }
                    }
                });
            }

            const Rect& GetRect() const
            {
                return mask.GetBoundingBox();
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
