#pragma once

namespace KinectVisionLib
{
    namespace Core
    {
        class DepthPixel
        {
        public:
            DepthPixel(int x, int y, int depth)
            {
                this->x = x;
                this->y = y;
                this->depth = depth;
            }

            int GetX() const { return x; }
            int GetY() const { return y; }
            int GetDepth() const { return depth; }

        private:
            int x;
            int y;
            int depth;
        };
    }
}