#pragma once

#include "../DepthImage.h"
#include "Chip.h"
#include "ChipCloud.h"
#include "MotionState.h"
#include "DeltaMotionState.h"

namespace KinectVisionLib
{
    namespace Core
    {
        class ChipTracker
        {
        public:
            ChipTracker(int id)
                : id(id)
            {}

            int GetId() { return id; }

            void UpdatePosition(shared_ptr<const DepthImage> depthFrame)
            {
                float score = 0.0;
                this->chip = FindMatch(depthFrame, deltaMotionState, &score);
            }

            bool IsActive() const { return isActive; }

        private:
            shared_ptr<Chip> FindMatch(shared_ptr<const DepthImage> depthImage, DeltaMotionState deltaMotionState, float* score)
            {
                return FindMatchWith12Ways(depthImage, deltaMotionState, score);
            }

            shared_ptr<Chip> FindMatchWith12Ways(shared_ptr<const DepthImage> depthImage, DeltaMotionState deltaMotionState, float* score)
            {
                // Iterate the search process
                return nullptr;
            }
            shared_ptr<ErrorMap> Match(shared_ptr<Chip> chip, shared_ptr<DepthImage> depthFrame, DeltaMotionState deltaMotionState)
            {
                return chip->Match(depthFrame, deltaMotionState);
            }
            //std::pair<double, MotionState> SearchResult(shared_ptr<DepthImage> depthFrame, Chip* chip, MotionState& motionState)
            //{
            //    return std::pair<double, MotionState>(chip->Match(depthFrame, motionState), motionState);
            //}

        private:
            int id;
            shared_ptr<Chip> chip;
            MotionState motionState;
            DeltaMotionState deltaMotionState;

            bool isActive;
        };
    }
}
