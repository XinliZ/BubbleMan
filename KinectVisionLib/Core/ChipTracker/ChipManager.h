#pragma once

#include "../DepthImage.h"
#include "../AreaMap.h"
#include "Chip.h"
#include "ChipTracker.h"
#include "../Segmentation/ImageSegmentation.h"

namespace KinectVisionLib
{
    namespace Core
    {
        // The manager component to follow all the active objects in the scene
        class ChipManager
        {
        public:
            ChipManager()
                : idSeed(0)
            {
            }

            void Update(shared_ptr<const DepthImage> depthFrame, shared_ptr<DepthImage> backgroundRemovedImage)
            {
                ImageSegmentation segment(10);
                auto areaMap = segment.SegmentImageWithMask(depthFrame, backgroundRemovedImage);
                list<shared_ptr<ChipTracker>> newTracker;
                for (int areaCode = 1; areaCode < areaMap->GetAreaCount(); areaCode++)
                {
                    bool foundMatching = false;
                    for (auto tracker : trackers)
                    {
                        // TODO: Here is only the rough matching. We have to drill into more details when there are
                        // multiple matching areas
                        if (tracker->Matches(areaMap->GetRect(areaCode)))
                        {
                            foundMatching = true;
                            tracker->UpdatePosition(depthFrame, areaMap, areaCode);
                        }
                    }
                    if (!foundMatching)
                    {
                        newTracker.push_back(make_shared<ChipTracker>(idSeed++, depthFrame, areaMap, areaCode));
                    }
                }
                trackers.insert(trackers.end(), newTracker.begin(), newTracker.end());
            }

            const std::list<shared_ptr<ChipTracker>> GetTrackers() const
            {
                return this->trackers;
            }

        private:
            int idSeed;
            std::list<shared_ptr<ChipTracker>> trackers;
        };
    }
}