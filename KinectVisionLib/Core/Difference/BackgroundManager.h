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
			}

			shared_ptr<DepthImage> Update(shared_ptr<DepthImage> image)
			{
				return SubstractAndUpdateBackground(image);
			}

		private:
			shared_ptr<DepthImage> SubstractAndUpdateBackground(shared_ptr<DepthImage> image)
			{
				double backgroundThreshold = 1.05;		// TODO: Should make this a configuration value
				shared_ptr<DepthImage> result = make_shared<DepthImage>(image->GetWidth(), image->GetHeight());
				for (int i = 0; i < image->GetHeight(); i++)
				{
					uint16* bgScan = this->background->GetScan0() + i * this->background->GetStride();
					uint16* imageScan = image->GetScan0() + i * image->GetStride();
					uint16* resultScan = result->GetScan0() + i * result->GetStride();
					for (int j = 0; j < image->GetWidth(); j++)
					{
						// TODO: We would need a gaussian model to tell if the point is a background point
						if (*imageScan == 0)
						{
							// Ignore invalid input point
						}
						else if (*bgScan == 0 || *imageScan > *bgScan)
						{
							// The new background exposed, update the background only
							*bgScan = *imageScan;
							*resultScan = 10;
						}
						else if ((*imageScan) * backgroundThreshold < *bgScan)
						{
							*resultScan = *imageScan;
						}

						bgScan++;
						imageScan++;
						resultScan++;
					}
				}
				return result;
			}

		private:
			shared_ptr<DepthImage> background;
		};
	}
}