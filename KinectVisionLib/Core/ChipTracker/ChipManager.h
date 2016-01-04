#pragma once

#include "../DepthImage.h"
#include "../AreaMap.h"
#include "Chip.h"
#include "ChipTracker.h"

namespace KinectVisionLib
{
    namespace Core
    {
        class ChipManager
        {
        public:
            ChipManager(shared_ptr<DepthImage> depthFrame, shared_ptr<AreaMap> areaMap)
            {
                // Initialize the chip manager with the areaMap and depthFrame
                //auto masks = areaMap->GenerateFrameMasks();
            }

            void Update(shared_ptr<DepthImage> depthFrame)
            {
                for (auto tracker : activeTracker)
                {
                    tracker.UpdatePosition(depthFrame);
                }
            }

        private:
            std::list<ChipTracker> activeTracker;
            std::list<ChipTracker> inactiveTracker;
        };
    }
}