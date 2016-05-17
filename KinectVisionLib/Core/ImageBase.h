#pragma once

namespace KinectVisionLib {
    namespace Core {

        using namespace std;

        class ImageBase
        {
        public:
            ImageBase(int width, int height)
            {
                this->width = width;
                this->height = height;
            }
            int GetWidth() const { return width; }
            int GetHeight() const { return height; }
            Rect GetRect() const { return Rect(0, 0, width, height); }
            Size GetSize() const { return Size(width, height); }
            Point GetCenter() const { return Point(width / 2, height / 2); }

            virtual void RenderAsRGBA(uint8* buffer) const = 0;
            virtual const wstring ReadPixelValue(int x, int y, int width, int height) const = 0;

        protected:
            bool IsValid(Point point) const {
                return Rect(0, 0, width, height).Contains(point);
            }

        private:
            int width;
            int height;
        };
    }
}