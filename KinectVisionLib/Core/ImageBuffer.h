#pragma once


namespace KinectVisionLib
{
    namespace Core
    {
        template<class T>
        class ImageBuffer : public Image<T>
        {
        public:
            ImageBuffer(int width, int height)
                : Image<T>(width, height)
            {}
            ImageBuffer(const Size& size)
                : Image<T>(size)
            {}

            void SetPixel(Point point, T pixel) {
                Image<T>::SetPixel(point, pixel);
            }

            void SetPixelSafe(Point point, T pixel) {
                Image<T>::SetPixelSafe(point, pixel);
            }

            virtual void ToDisplay(uint8* buffer) const override {
                // Do nothing here
            }
        };
    }
}