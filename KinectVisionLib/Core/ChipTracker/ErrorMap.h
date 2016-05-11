#pragma once

#include "../Image.h"

namespace KinectVisionLib
{
    namespace Core
    {
        class ErrorMap : public Image<int16>
        {
        public:
            ErrorMap(const Size& size)
                : Image<int16>(size)
            {
            }

            void SetPixelInvalid(Point point)
            {
                *(GetScan0() + point.GetY() * GetStride() + point.GetX()) = invalidBits;
            }

            virtual void RenderAsRGBA(uint8* buffer) const override 
            {
                ImageOperation([buffer, this](int x, int y, int16 pixel) {
                    int index = (y * GetWidth() + x) * 4;
                    if (pixel == invalidBits)        // Invalid pixel
                    {
                        buffer[index + 1] = 0xFF;
                        buffer[index + 3] = 0xFF;
                    }
                    else if (pixel > 0)
                    {
                        buffer[index] = pixel & 0xFF;
                        buffer[index + 3] = 0xFF;
                    }
                    else
                    {
                        buffer[index + 2] = (-pixel) & 0xFF;
                        buffer[index + 3] = 0xFF;
                    }
                });
            }

        public:
            float AnalyzeResult()
            {
                // Histogram and binarization
                //for ()
                return 0.0f;
            }

            float TestXDirection() const
            {
                //for (int i = 0; i < )
                return 0.0f;
            }

            float TestYDirection() const
            {
                return 0.0f;
            }

            float TestZDirection() const
            {
                // Test zoom in/out
                return 0.0f;
            }

        private:
            //Rect area;
            const int16 invalidBits = (int16)0x8000;
        };
    }
}