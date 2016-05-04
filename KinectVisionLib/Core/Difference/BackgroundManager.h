#pragma once

#include "../DepthImage.h"

namespace KinectVisionLib
{
    namespace Core
    {
        using namespace std;

        class BackgroundManager
        {
        public:
            BackgroundManager(shared_ptr<const DepthImage> initialImage)
            {
                this->background = make_shared<DepthImage>(*initialImage.get());
                this->backgroundAccumulationMask = make_shared<DepthImage>(*initialImage.get());
            }

            shared_ptr<DepthImage> Update(shared_ptr<const DepthImage> image)
            {
                return SubstractAndUpdateBackground(image);
            }

            shared_ptr<const DepthImage> GetAccumulatedBackground() const
            {
                return this->background;
            }

        private:
            shared_ptr<DepthImage> SubstractAndUpdateBackground(shared_ptr<const DepthImage> image)
            {
                float backgroundThreshold = 0.95f;        // TODO: Should make this a configuration value
                shared_ptr<DepthImage> result = make_shared<DepthImage>(image->GetWidth(), image->GetHeight());
                background->ImageOperation<uint16, uint16, uint16>(image, result, backgroundAccumulationMask, [backgroundThreshold](uint16* bgScan, const uint16* inputScan, uint16* resultScan, uint16* accumulationMaskScan) {
                    if (*inputScan == 0)
                    {
                        // Ignore invalid input point
                    }
                    else if (*bgScan == 0)
                    {
                        *bgScan = *inputScan;
                        *resultScan = 0;
                    }
                    else if (*inputScan >= (*bgScan) * backgroundThreshold)
                    {
                        if (*inputScan > *bgScan)
                        {
                            *accumulationMaskScan += 2;
                            if (*accumulationMaskScan > 5)    // 4 accumulations, for this case
                            {
                                *bgScan = *inputScan;
                                *resultScan = 0;
                            }
                        }
                    }
                    else
                    {
                        // Non-background
                        *resultScan = *inputScan;
                    }
                });

                backgroundAccumulationMask->ImageOperation([](uint16* scan) {
                    if (scan > 0) (*scan)--;
                });
                return result;
            }

        private:
            shared_ptr<DepthImage> background;
            shared_ptr<DepthImage> backgroundAccumulationMask;
        };
    }
}