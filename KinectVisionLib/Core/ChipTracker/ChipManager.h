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

            void Update(shared_ptr<DepthImage> depthFrame, shared_ptr<DepthImage> backgroundRemovedImage)
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
            void DetectAndAddNewTrackers(std::list<shared_ptr<ChipTracker>> trackers, shared_ptr<DepthImage> depthFrame, shared_ptr<DepthImage> backgroundRemovedImage)
            {
                ImageSegmentation segment(10);
                auto result = segment.SegmentImageWithMask(depthFrame, backgroundRemovedImage);
                std::list<shared_ptr<ChipTracker>> newTrackers;
                
                // TODO: To be finished
                //for (auto area : result.GetAreaList())
                //{
                //    if (none_of(trackers.begin(), trackers.end(), [](ChipTracker tracker){ tracker.Matches(area); }))
                //    {
                //        newTrackers.push_back(make_shared<ChipTracker>(idSeed++));      // TODO: We will also need the region
                //    }
                //}
            }

        private:
            int idSeed;
            std::list<shared_ptr<ChipTracker>> trackers;
        };
    }
}