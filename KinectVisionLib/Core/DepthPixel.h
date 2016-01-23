#pragma once

#include "Vector3.h"

namespace KinectVisionLib
{
    namespace Core
    {
        class DepthPixel
        {
        public:
            // x, y is according to the origin in center of image
            DepthPixel(int x, int y, int depth)
            {
                this->x = x;
                this->y = y;
                this->depth = depth;
            }
            DepthPixel(const DepthPixel& dp)
            {
                this->x = dp.x;
                this->y = dp.y;
                this->depth = dp.depth;
            }
            DepthPixel(const Vector3& v)
            {
                this->x = (int)(v.GetX() * factor / v.GetZ());
                this->y = (int)(v.GetY() * factor / v.GetZ());
                this->depth = (int)v.GetZ();
            }

            Vector3 ToVector3() const
            {
                return Vector3(x * depth / factor, y * depth / factor, (float)depth);
            }

            int GetX() const { return x; }
            int GetY() const { return y; }
            int GetDepth() const { return depth; }

        private:
            int x;
            int y;
            int depth;

            const float factor = 500.f;
        };
    }
}