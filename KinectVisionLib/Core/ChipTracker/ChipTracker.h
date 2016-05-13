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
            ChipTracker(int id, shared_ptr<const DepthImage> depthFrame, shared_ptr<const AreaMap> areaMap, uint16 areaCode)
                : id(id)
                , chip(make_shared<Chip>(depthFrame, areaMap, areaCode))
                , isActive(true)
            {
            }

            int GetId() const { return id; }

            bool IsActive() const { return isActive; }

            void UpdatePosition(shared_ptr<const DepthImage> depthFrame, shared_ptr<const AreaMap> areaMap, uint16 areaCode)
            {
                float score = 0.0;
                this->chip = FindMatch(depthFrame, deltaMotionState, &score, areaMap, areaCode);
            }

            // TODO: Implementation needed
            // We could use bounding box here to match
            bool Matches(const Rect& area) const {
                return area.OverlapWith(chip->GetRect());
            }

        private:

            shared_ptr<Chip> FindMatch(shared_ptr<const DepthImage> depthImage, DeltaMotionState& deltaMotionState, float* score, shared_ptr<const AreaMap> areaMap, uint16 areaCode)
            {
                const int maxIteration = 10;
                const float iterationThreshold = 30.0f;
                for (int i = 0; i < maxIteration; i++)
                {
                    ErrorMap errorMap(depthImage->GetSize());
                    chip->Match(depthImage, deltaMotionState, errorMap);
                    float score = errorMap.GetMeanSquareError();
                    if (score < iterationThreshold)
                    {
                        // Matching succeeded
                        break;
                    }
                    else
                    {
                        // TODO: Refine the parameters based on the errorMap and match again
                        UpdateTranslationParameter(deltaMotionState, errorMap);
                        UpdateRotationParameter(deltaMotionState, errorMap);
                    }
                }
                return make_shared<Chip>(depthImage, areaMap, areaCode);
            }

            void UpdateTranslationParameter(DeltaMotionState& deltaMotionState, const ErrorMap& errorMap)
            {
                float dx = errorMap.TestXDirection();
                float dy = errorMap.TestYDirection();
                float dz = errorMap.TestZDirection();
                deltaMotionState.Translate(dx, dy, dz);
            }

            void UpdateRotationParameter(DeltaMotionState& deltaMotionState, const ErrorMap& errorMap)
            {
                // TODO: To be implemented
            }

            shared_ptr<Chip> FindMatchWith12Ways(shared_ptr<const DepthImage> depthImage, DeltaMotionState& deltaMotionState, float* score)
            {
                // Iterate the search process
                return nullptr;
            }

        private:
            int id;
            shared_ptr<Chip> chip;      // TODO: Should tracker keep a list of chips?

            MotionState motionState;
            DeltaMotionState deltaMotionState;

            bool isActive;
        };
    }
}
