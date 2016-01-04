#include "pch.h"
#include "KinectVisionManager.h"

using namespace KinectVisionLib::Core;

KinectVisionManager::KinectVisionManager()
{
}


KinectVisionManager::~KinectVisionManager()
{
}

void KinectVisionManager::UpdateBlackDotsMask(shared_ptr<DepthImage> image, shared_ptr<DepthImage> newImage)
{
	for (int i = 0; i < image->GetHeight(); i++)
	{
		uint16* target = image->GetScan0() + i * image->GetStride();
		uint16* source = newImage->GetScan0() + i * newImage->GetStride();
		for (int j = 0; j < image->GetWidth(); j++)
		{
			*target += (*source == 0) ? 15 : 0;
			if (*target > 0) *target--;

			target++;
			source++;
		}
	}
}