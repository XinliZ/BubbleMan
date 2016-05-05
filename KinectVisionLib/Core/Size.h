#pragma once

namespace KinectVisionLib
{
    namespace Core
    {
        union Size
        {
        public:
            Size(int width, int height)
            {
                this->width = width;
                this->height = height;
            }

            Size(const Size& size)
            {
                this->data = size.data;
            }

            int GetWidth() const { return width; }
            int GetHeight() const { return height; }
            bool operator > (Size& size) const { return this->width > size.width && this->height > size.height; }
            bool operator < (Size& size) const { return this->width < size.width && this->height < size.height; }
            bool operator == (Size& size) const { return this->data == size.data; }

        private:
            uint64 data;
            struct {
                int32 width;
                int32 height;
            };
        };
    }
}