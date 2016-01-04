#pragma once

#include "Point.h"
#include "Size.h"

namespace KinectVisionLib
{
	namespace Core
	{
		class Rect
		{
		public:
			Rect(int left, int top, int width, int height)
				:offset(left, top), size(width, height)
			{
			}

			Rect(Point offset, Size size)
				:offset(offset), size(size)
			{
			}

			Point GetOffset() const { return offset; }
			Size GetSize() const { return size; }
			bool IsInside(Point point) const { return point.GetX() >= offset.GetX() && point.GetY() >= offset.GetY() && point.GetX() < offset.GetX() + size.GetWidth() && point.GetY() < offset.GetY() + size.GetHeight(); }

			// Extend the rect on all directions
			Rect Extend(int dLeft, int dTop, int dRight, int dButtom) const { return Rect(offset.Offset(-dLeft, -dTop), Size(size.GetWidth() + dLeft + dRight, size.GetHeight() + dTop + dButtom)); }

		private:
			Point offset;
			Size size;
		};
	}
}