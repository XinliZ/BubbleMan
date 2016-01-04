#pragma once

#include "../Image.h"

namespace KinectVisionLib
{
	namespace Core
	{
		class FrameMask : public Image<bool>
		{
		public:
			FrameMask(Rect rect)
				: Image<bool>(rect.GetSize()), boundingBox(rect)
			{}

			virtual void ToDisplay(uint8* buffer) const
			{
				// Do nothing, we don't need this for now
			}

			bool IsPixelValid(Point point)
			{
				return boundingBox.IsInside(point) ? GetPixel(point.Offset(boundingBox.GetOffset())) : false;
			}
		private:
			Rect boundingBox;
		};

	}
}

