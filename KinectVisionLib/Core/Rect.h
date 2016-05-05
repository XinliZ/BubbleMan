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
            int32 GetLeft() const { return offset.GetX(); }
            int32 GetTop() const { return offset.GetY(); }
            int32 GetWidth() const { return size.GetWidth(); }
            int32 GetHeight() const { return size.GetHeight(); }
            int32 GetRight() const { return offset.GetX() + size.GetWidth(); }
            int32 GetBottom() const { return offset.GetY() + size.GetHeight(); }

            bool Contains(const Point& point) const 
            { 
                return point.GetX() >= offset.GetX() && point.GetY() >= offset.GetY() && point.GetX() < offset.GetX() + size.GetWidth() && point.GetY() < offset.GetY() + size.GetHeight();
            }

            bool operator <= (const Size& size) const
            {
                return offset.GetX() >= 0 && offset.GetY() >= 0 && GetRight() <= size.GetWidth() && GetBottom() <= size.GetHeight();
            }

            // Extend the rect on all directions
            Rect Extend(int dLeft, int dTop, int dRight, int dButtom) const 
            { 
                return Rect(offset.Offset(-dLeft, -dTop), Size(size.GetWidth() + dLeft + dRight, size.GetHeight() + dTop + dButtom)); 
            }
            Rect Extend(Point point)
            {
                return Rect(min(point.GetX(), offset.GetX()), 
                    min(point.GetY(), offset.GetY()), 
                    max(point.GetX() - offset.GetX(), size.GetWidth()), 
                    max(point.GetY() - offset.GetY(), size.GetHeight()));
            }

        private:
            Point offset;
            Size size;
        };
    }
}