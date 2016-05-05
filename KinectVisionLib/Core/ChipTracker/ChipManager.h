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
                shared_ptr<AreaMap> areaMap = make_shared<AreaMap>(depthFrame->GetWidth(), depthFrame->GetHeight());
                for (auto tracker : trackers)
                {
                    if (tracker->IsActive())
                    {
                        tracker->UpdatePosition(depthFrame);
                        //areaMap->Mark(tracker);
                    }
                }
                
                DetectAndAddNewTrackers(trackers, depthFrame, backgroundRemovedImage);
            }

            const std::list<shared_ptr<ChipTracker>> GetTrackers() const
            {
                return this->trackers;
            }

        private:
            void DetectAndAddNewTrackers(std::list<shared_ptr<ChipTracker>> trackers, shared_ptr<const DepthImage> depthFrame, shared_ptr<DepthImage> backgroundRemovedImage)
            {
                ImageSegmentation segment(10);
                auto areaMap = segment.SegmentImageWithMask(depthFrame, backgroundRemovedImage);
                std::list<shared_ptr<ChipTracker>> newTrackers;
                
                for (int areaCode = 1 /*areaCode 0 is preserved*/; areaCode < areaMap->GetAreaCount(); areaCode++)
                {
                    if (none_of(trackers.begin(), trackers.end(), [areaMap, areaCode](shared_ptr<ChipTracker> tracker){ return tracker->Matches(areaMap->GetRect(areaCode));}))
                    {
                        newTrackers.push_back(make_shared<ChipTracker>(idSeed++, areaMap, areaCode));      // TODO: We will also need the region
                    }
                }
            }

        private:
            int idSeed;
            std::list<shared_ptr<ChipTracker>> trackers;
        };
    }
}