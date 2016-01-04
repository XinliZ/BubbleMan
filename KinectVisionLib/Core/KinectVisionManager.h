#pragma once

#include "Image.h"
#include "AreaMap.h"
#include "DifferencialImage.h"
#include "Segmentation/ImageSegmentation.h"
#include "Difference/BackgroundManager.h"
#include "ChipTracker/ChipManager.h"

namespace KinectVisionLib{
    namespace Core{

        using namespace std;

        class KinectVisionManager
        {
        public:
            KinectVisionManager();
            ~KinectVisionManager();

            shared_ptr<Image<uint16>> FeedFrame(shared_ptr<DepthImage> image)
            {
                this->frameCount++;

                //return SegmentEveryFrame(image);
                return SegmentThenTracking(image);
                //return ImageSubstraction(image);
            }

        private:
            void UpdateBlackDotsMask(shared_ptr<DepthImage> image, shared_ptr<DepthImage> newImage);

            shared_ptr<Image<uint16>> SegmentThenTracking(shared_ptr<DepthImage> image)
            {
                shared_ptr<Image<uint16>> result;
                // Build tracker list from segmentation results
                // Update the tracker with new image
                if (backgroundManager == nullptr)
                {
                    // First frame, initialize the background frame
                    backgroundManager = make_shared<BackgroundManager>(image);
                    result = image;
                }
                else
                {
                    auto backgroundImage = backgroundManager->Update(image);
                    ImageSegmentation segment(10);
                    result = segment.SegmentImageWithMask(image, backgroundImage);

                }
                return result;
            }

            // Do segmentation on each frame based on the background results
            shared_ptr<Image<uint16>> SegmentWithBackground(shared_ptr<DepthImage> image)
            {
                shared_ptr<Image<uint16>> result;
                // Build tracker list from segmentation results
                // Update the tracker with new image
                if (backgroundManager == nullptr)
                {
                    // First frame, initialize the background frame
                    backgroundManager = make_shared<BackgroundManager>(image);
                    result = image;
                }
                else
                {
                    auto backgroundImage = backgroundManager->Update(image);
                    ImageSegmentation segment(10);
                    result = segment.SegmentImageWithMask(image, backgroundImage);
                }
                return result;
            }

            shared_ptr<Image<uint16>> SegmentEveryFrame(shared_ptr<DepthImage> image)
            {
                ImageSegmentation segment(10);
                return segment.SegmentImage(image);
            }

            shared_ptr<Image<uint16>> ImageSubstraction(shared_ptr<DepthImage> image)
            {
                if (blackDotsMask == nullptr)
                {
                    blackDotsMask = make_shared<DifferencialImage>(image->GetWidth(), image->GetHeight());
                }
                this->frameCount++;
                shared_ptr<Image<uint16>> result = nullptr;
                if (this->previousImage == nullptr)
                {
                    //imageList.push_front(image);
                    this->previousImage = image;
                    //this->currentImage = image;
                    result = image;
                }
                else
                {
                    //UpdateBlackDotsMask(this->blackDotsMask, image);

                    //// Image substraction
                    //shared_ptr<Image> previousImage = this->previousImage;    // *imageList.begin();
                    //result = image->Apply([](uint16* target, uint16* source1, uint16* source2, uint16* blackDotsMask){
                    //    uint16 midLine = 0x8000;
                    //    if (0)//*source1 == 0 || *source2 == 0)
                    //    {
                    //        *target = midLine;
                    //    }
                    //    else
                    //    {
                    //        *target = midLine - *source2 + *source1;
                    //    }
                    //}, previousImage, this->blackDotsMask);

                    this->previousImage = image;    //imageList.push_front(image);
                }
                return result;
            }

        private:
            //list<shared_ptr<Image>> imageList;
            shared_ptr<DepthImage> previousImage;
            shared_ptr<DepthImage> currentImage;

            // Record the black dots (invalid depth pixels) for a few frames and use them to remove the noises.
            shared_ptr<DifferencialImage> blackDotsMask;            // Deprecated. Use background manager
            shared_ptr<BackgroundManager> backgroundManager;
            shared_ptr<ChipManager> chipManager;

            int frameCount = 0;
        };

    }
}
