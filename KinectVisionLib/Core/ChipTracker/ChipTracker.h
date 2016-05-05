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
            ChipTracker(int id, shared_ptr<const AreaMap> map, uint16 areaCode)
                : id(id)
            {
                map->GetRect(areaCode);
            }

            int GetId() { return id; }

            void UpdatePosition(shared_ptr<const DepthImage> depthFrame)
            {
                float score = 0.0;
                this->chip = FindMatch(depthFrame, deltaMotionState, &score);
            }

            bool IsActive() const { return isActive; }

            // TODO: Implementation needed
            // We could use bounding box here to match
            bool Matches(const Rect& area) const { return false; }

        private:
            shared_ptr<Chip> FindMatch(shared_ptr<const DepthImage> depthImage, DeltaMotionState deltaMotionState, float* score)
            {
                const int maxIteration = 10;
                const float iterationThreshold = 30.0f;
                for (int i = 0; i < maxIteration; i++)
                {
                    auto errorMap = Match(chip, depthImage, deltaMotionState);
                    if (errorMap->GetScore() < iterationThreshold)
                    {
                        // Matching succeeded
                        break;
                    }
                    else
                    {
                        // TODO: Refine the parameters based on the errorMap and match again
                    }
                }
                // TODO: After the alignment, we will need segmentation in the area
                return make_shared<Chip>(depthImage, shared_ptr<AreaMap>(), 0); // TODO: Fill up the parameters
            }

            shared_ptr<Chip> FindMatchWith12Ways(shared_ptr<const DepthImage> depthImage, DeltaMotionState& deltaMotionState, float* score)
            {
                // Iterate the search process
                return nullptr;
            }
            shared_ptr<const ErrorMap> Match(shared_ptr<Chip> chip, shared_ptr<const DepthImage> depthFrame, DeltaMotionState& deltaMotionState)
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
