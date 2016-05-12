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
                    if (pixel == invalidBits)        // Invalid pixel is green
                    {
                        buffer[index + 1] = 0xFF;
                        buffer[index + 3] = 0xFF;
                    }
                    else if (pixel > 0)
                    {
                        buffer[index] = pixel > 0xFF ? 0xFF : pixel;
                        buffer[index + 3] = 0xFF;
                    }
                    else if (pixel < 0)
                    {
                        buffer[index + 2] = (-pixel) > 0xFF ? 0xFF : (-pixel);
                        buffer[index + 3] = 0xFF;
                    }
                    else
                    {
                        buffer[index + 3] = 0xFF;
                    }
                });
            }

        public:
            float AnalyzeResult() { return 0.0f; }
            void AnalyzeResults()
            {
                float squareError = 0.0f;
                float totalPositive = 0.0f;
                float totalNegative = 0.0f;
                float count = 0;
                float positiveCount = 0;
                float negativeCount = 0;

                static_cast<const ErrorMap*>(this)->ImageOperation([&](int x, int y, int16 pixel) {
                    if (pixel != invalidBits)
                    {
                        squareError += pixel * pixel;
                        if (pixel > 0) {
                            totalPositive += pixel;
                            positiveCount++;
                        }
                        else if (pixel < 0) {
                            totalNegative -= pixel;
                            negativeCount++;
                        }

                        count++;
                    }
                });
                
                this->meanSquareError = squareError / count;
                this->positiveError = totalPositive / positiveCount;
                this->negativeError = totalNegative / negativeCount;
            }

            float GetMeanSquareError() const { return this->meanSquareError; }
            float GetPositiveError() const { return this->positiveError; }
            float GetNegativeError() const { return this->negativeError; }

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

            float meanSquareError;
            float positiveError;
            float negativeError;
        };
    }
}