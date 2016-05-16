#pragma once

#include "GlobalConsts.h"
#include "Image.h"
#include "AreaMap.h"
#include "DifferencialImage.h"
#include "Segmentation/ImageSegmentation.h"
#include "Difference/BackgroundManager.h"
#include "ChipTracker/ChipManager.h"
#include "ProcessContext.h"

namespace KinectVisionLib{
    namespace Core{

        using namespace std;

        class KinectVisionManager
        {
        public:
            KinectVisionManager();
            ~KinectVisionManager();

            shared_ptr<const Image<uint16>> FeedFrame(shared_ptr<const DepthImage> image)
            {
                this->frameCount++;
                this->context.Clear();

                return SegmentThenTracking(image);
                //return SegmentEveryFrame(image);
                //return ImageSubstraction(image);
                //return SegmentWithBackground(image);
            }

            ProcessContext* GetProcessContext()
            {
                return &context;
            }

            shared_ptr<const ErrorMap> TransformFrame(shared_ptr<const DepthImage> image, shared_ptr<const DepthImage> previousImage, 
                float dX, float dY, float dZ, float dA, float dB, float dR) const;

            shared_ptr<const ErrorMap> CreateNormalMap(shared_ptr<const DepthImage> image, int direction);

        private:
            void UpdateBlackDotsMask(shared_ptr<DepthImage> image, shared_ptr<DepthImage> newImage);

            shared_ptr<const Image<uint16>> SegmentThenTracking(shared_ptr<const DepthImage> image)
            {
                shared_ptr<const Image<uint16>> result;
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
                    auto backgroundRemovedImage = backgroundManager->Update(image);
                    context.AddDebugFrame(L"BackgroundDiff", backgroundRemovedImage);
                    context.AddDebugFrame(L"Background", backgroundManager->GetAccumulatedBackground());

                    ImageSegmentation segment(10);
                    result = segment.SegmentImageWithMask(image, backgroundRemovedImage);

                    this->chipManager.Update(image, backgroundRemovedImage);
                }
                return result;
            }

            // Do segmentation on each frame based on the background results
            shared_ptr<const Image<uint16>> SegmentWithBackground(shared_ptr<const DepthImage> image)
            {
                shared_ptr<const Image<uint16>> result;
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
            shared_ptr<ErrorMap> MatchImages(const DepthImage* image, const DepthImage* previousImage, 
                float dX, float dY, float dZ, float dA, float dB, float dR) const;

            void UpdateParametersOnErrorMap(ErrorMap* errorMap, float dX, float dY, float dZ, float dA, float dB, float dR) const;

        private:
            //list<shared_ptr<Image>> imageList;
            shared_ptr<DepthImage> previousImage;
            shared_ptr<DepthImage> currentImage;

            // Record the black dots (invalid depth pixels) for a few frames and use them to remove the noises.
            shared_ptr<DifferencialImage> blackDotsMask;            // Deprecated. Use background manager
            shared_ptr<BackgroundManager> backgroundManager;
            ChipManager chipManager;

            int frameCount = 0;

            ProcessContext context;
        };

    }
}
