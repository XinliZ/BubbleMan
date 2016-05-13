#pragma once

#include "../GlobalConsts.h"
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
                    pixel *= 2;
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
            float GetMeanSquareError() const { return this->meanSquareError; }
            float GetPositiveError() const { return this->positiveError; }
            float GetNegativeError() const { return this->negativeError; }
            float GetXOffset() const { return this->xOffset; }
            float GetYOffset() const { return this->yOffset; }

            void AnalyzeResults(const DepthImage* img)
            {
                MeasureErrors(this->meanSquareError, this->positiveError, this->negativeError);

                PredictXYOffset(img, this->xOffset, this->yOffset);
            }

        private:
            void MeasureErrors(float& meanSquareError, float& positiveError, float& negativeError) const
            {
                float squareError = 0.0f;
                float totalPositive = 0.0f;
                float totalNegative = 0.0f;
                float count = 0;
                float positiveCount = 0;
                float negativeCount = 0;

                ImageOperation([&](int x, int y, int16 pixel) {
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

                meanSquareError = squareError / count;
                positiveError = totalPositive / positiveCount;
                negativeError = totalNegative / negativeCount;
            }

            void PredictXYOffset(const DepthImage* img, float& xOffset, float& yOffset) const
            {
                float totalXOffset = 0.0f;
                float totalYOffset = 0.0f;
                float count = 0.0f;
                NormalOperation<uint16>(img, [&](Vector3 v, int16 error) {
                    auto result = v * error;
                    totalXOffset += v.GetX();
                    totalYOffset += v.GetY();
                    count++;
                });
                xOffset = totalXOffset / count;
                yOffset = totalYOffset / count;
            }

        public:

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

            float xOffset;
            float yOffset;
        };
    }
}