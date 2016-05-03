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
            BackgroundManager(shared_ptr<DepthImage> initialImage)
            {
                this->background = make_shared<DepthImage>(*initialImage.get());
				this->backgroundAccumulationMask = make_shared<DepthImage>(*initialImage.get());
            }

            shared_ptr<DepthImage> Update(shared_ptr<DepthImage> image)
            {
                return SubstractAndUpdateBackground(image);
            }

        private:
			shared_ptr<DepthImage> SubstractAndUpdateBackground(shared_ptr<DepthImage> image)
			{
				double backgroundThreshold = 1.05;        // TODO: Should make this a configuration value
				shared_ptr<DepthImage> result = make_shared<DepthImage>(image->GetWidth(), image->GetHeight());
				background->ImageOperation<uint16, uint16>(image, result, [backgroundThreshold](uint16* bgScan, uint16* inputScan, uint16* resultScan) {
					if (*inputScan == 0)
					{
						// Ignore invalid input point
					}
					else if (*bgScan == 0 || *inputScan > *bgScan)
					{
						*bgScan = *inputScan;
						*resultScan = 10;
					}
					else if ((*inputScan) * backgroundThreshold < *bgScan)
					{
						*resultScan = *bgScan;
					}
				});
				return result;
			}

        private:
            shared_ptr<DepthImage> background;
			shared_ptr<DepthImage> backgroundAccumulationMask;
        };
    }
}