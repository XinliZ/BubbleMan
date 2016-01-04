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
                for (int i = 0; i < frame->GetHeight(); i++)
                {
                    const uint16* scan = frame->GetScan0() + i * frame->GetStride();
                    for (int j = 0; j < frame->GetWidth(); j++)
                    {
                        if (mask.IsPixelValid(Point(j, i)))
                        {
                            DepthPixel outPixel = deltaMotionState.TransformDepthPixel(center, DepthPixel(j, i, *scan));
                            auto pixel = depthFrame->GetPixelSafe(Point(outPixel.GetX(), outPixel.GetY()));
                            auto error = pixel - outPixel.GetDepth();
                        }
                    }
                }
            }

            //Chip(const Chip& chip, MotionState& motionState)
            //    : mask(Rect(0, 0, 0, 0))            // TODO: Fix this
            //{
            //    auto frame = ref new Frame();
            //    for (int i = 0; i < width; i++)
            //    {
            //        for (int j = 0; j < height; j++)
            //        {
            //            if (chip->depthArea->isValid(i, j))
            //            {
            //                motionState.TransformDepthPixel(i, j, depth, &x, &y, &newDepth);
            //                frame->SetPixel(x, y, newDepth);
            //                depthArea->SetPixel(x, y);
            //                ExtendBoundingBox(x, y);
            //            }
            //        }
            //    }

            //    this->frame = frame;
            //}
            //std::map<double, MotionState*>* SearchResults(Frame* frame, Chip* chip, MotionState* motionState);
            //std::pair<double, MotionState*> SearchResult(Frame* frame, Chip* chip, MotionState* motionState);

        private:
            shared_ptr<DepthImage> frame;
            FrameMask mask;
            DepthPixel center;

            DeltaMotionState offset;
            DeltaMotionState totalOffset;
            //Frame* frame;
        };
    }
}
