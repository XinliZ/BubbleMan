#pragma once

#include "../GlobalConsts.h"
#include "../Image.h"

namespace KinectVisionLib
{
    namespace Core
    {
        using namespace std;

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

            virtual const std::wstring ReadPixelValue(int x, int y, int width, int height) const override
            {
                float sum = 0;
                float count = 0;
                ImageOperation(Rect(x, y, width, height), [&sum, &count, this](int x, int y, int16 pixel) {
                    if (pixel != invalidBits)
                    {
                        sum += pixel;
                        count++;
                    }
                });
                float result = count > 0 ? sum / count : 0;
                return std::to_wstring(result);
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

                PredictXYOffset1(img, this->xOffset, this->yOffset);
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
                img->NormalOperation<int16>(this, [&](Vector3 v, int16 error) {
                    if (error != invalidBits)
                    {
                        auto result = v * error;
                        totalXOffset += v.GetX();
                        totalYOffset += v.GetY();
                        count++;
                    }
                });
                xOffset = totalXOffset / count;
                yOffset = totalYOffset / count;
            }

            void PredictXYOffset1(const DepthImage* img, float& xOffset, float& yOffset)
            {
                vector<pair<float, float>> results;
                img->AngleOperation(this, [&results, this](const Vector3& angles, int16 error) {
                    if (error != invalidBits)
                    {
                        // TODO: Dealing the x direction only for now
                        float a = tan(angles.GetX());
                        float b = error;
                        if (a != 0)
                        {
                            results.push_back(pair<float, float>(b / a, 0.0f));
                        }
                    }
                });

                //vector<Point> intersections;
                //for (int i = 0; i < results.size() - 1; i += 100)
                //{
                //    for (int j = i + 1; j < results.size(); j += 100)
                //    {
                //        Point point(0, 0);
                //        if (Intersection(results[i].first, results[i].second, results[j].first, results[j].second, point))
                //        {
                //            intersections.push_back(point);
                //        }
                //    }
                //}

                //float totalX = 0;
                //float totalY = 0;
                //for (Point p : intersections)
                //{
                //    totalX += p.GetX();
                //    totalY += p.GetY();
                //}

                //xOffset = totalX / intersections.size();
                //yOffset = totalY / intersections.size();
                float total = 0;
                for (auto p : results)
                {
                    total += p.first;
                }
                xOffset = total / results.size();
                yOffset = 123.0f;
            }

            bool Intersection(float A1, float B1, float A2, float B2, Point& point) const
            {
                if (A1 == A2)
                {
                    return false;
                }
                float x = (B2 - B1) / (A1 - A2);
                point = Point(x, x * A1 + B1);      // TODO: Should we use float here?
                return true;
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