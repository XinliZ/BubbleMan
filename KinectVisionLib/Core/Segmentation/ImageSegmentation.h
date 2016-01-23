#pragma once

#include "../AreaMap.h"
#include "../DepthImage.h"

namespace KinectVisionLib{
    namespace Core{

        using namespace std;

        class ImageSegmentation
        {
        public:
            ImageSegmentation(int borderThreshold):borderThreshold(borderThreshold) {}
            ~ImageSegmentation() {};

            shared_ptr<AreaMap> SegmentImage(shared_ptr<DepthImage> image)
            {
                this->image = image;
                this->areaMap = make_shared<AreaMap>(image->GetWidth(), image->GetHeight());

                AddSeeds(seeds);

                Iterate(nullptr);
                return this->areaMap;
            }

            shared_ptr<AreaMap> SegmentImageWithMask(shared_ptr<DepthImage> image, shared_ptr<DepthImage> mask)
            {
                this->image = image;
                this->areaMap = make_shared<AreaMap>(image->GetWidth(), image->GetHeight());

                AddSeeds(seeds, mask);

                Iterate(mask);
                return this->areaMap;
            }

        private:
            void Iterate(shared_ptr<DepthImage> mask)
            {
                while (!seeds.empty())
                {
                    Point point = seeds.front();
                    seeds.pop_front();

                    if (areaMap->IsOpen(point) && IsValidPoint(point))
                    {
                        GrowArea(point, mask);    
                    }
                }
            }

            bool IsValidPoint(Point point)
            {
                // Invalid depth pixel
                return image->GetPixel(point) != 0;
            }

            void AddSeeds(deque<Point>& seeds)
            {
                // Start with center of the image
                seeds.push_back(Point(image->GetWidth() / 2, image->GetHeight() / 2));

                for (int i = 3; i < image->GetHeight() - 3; i += 20)
                {
                    for (int j = 3; j < image->GetWidth() - 3; j += 20)
                    {
                        seeds.push_back(Point(j, i));
                    }
                }
            }

            void AddSeeds(deque<Point>& seeds, shared_ptr<DepthImage> mask)
            {
                // Start with center of the image
                AddSeedIfValid(seeds, Point(image->GetWidth() / 2, image->GetHeight() / 2), mask);

                for (int i = 3; i < image->GetHeight() - 3; i += 20)
                {
                    for (int j = 3; j < image->GetWidth() - 3; j += 20)
                    {
                        AddSeedIfValid(seeds, Point(j, i), mask);
                    }
                }
            }

            void AddSeedIfValid(deque<Point>& seeds, Point point, shared_ptr<DepthImage> mask)
            {
                if (mask->Test5x5Window(point, [](uint16 pixel){
                    return pixel == 0 ? 0 : 1;
                }, [](int count){return count > 20; }))     // TODO: Adjustable thresholds: The ratial to place the seed point
                {
                    seeds.push_back(point);
                }
            }

            void GrowArea(Point point, shared_ptr<DepthImage> mask)
            {
                uint16 areaCode = areaMap->CreateNewAreaCode();

                GrowArea(point, areaCode, mask);
            }

            void GrowArea(Point point, uint16 areaCode, shared_ptr<DepthImage> mask)
            {
                auto imageRect = image->GetRect().Extend(-1, -1, -1, -1);
                int backgroundPixelCount = 0;
                int areaPixelCount = 0;
                deque<Point> growingSeeds;
                //growingSeeds.reserve(2048);        // Random number
                growingSeeds.push_back(point);

                while (!growingSeeds.empty())
                {
                    auto seed = *growingSeeds.begin();
                    growingSeeds.pop_front();

                    // Leave 1 pixel edge to make sure the pixel read/write safe
                    if (imageRect.IsInside(seed))
                    {
                        areaPixelCount++;
                        if (mask->GetPixel(seed) == 0)
                        {
                            backgroundPixelCount++;
                        }

                        // 0: Center pixel, 1: left, 2: top, 3: right, 4: bottom
                        uint16 pixelValues[5];
                        ReadSurroundingPixels(pixelValues, seed);


                        TestPixelOnLeft(pixelValues, seed.LeftNeighbor(), areaCode, growingSeeds);
                        TestPixelOnTop(pixelValues, seed.UpNeighbor(), areaCode, growingSeeds);
                        TestPixelOnRight(pixelValues, seed.RightNeighbor(), areaCode, growingSeeds);
                        TestPixelOnBottom(pixelValues, seed.DownNeighbor(), areaCode, growingSeeds);
                    }
                }

                // TODO: Too slow
                //if (backgroundPixelCount > areaPixelCount * 0.8f)          // Adjustable threshold
                //{
                //    this->areaMap->ReplaceAreaCode(areaCode, 0);
                //}
            }

            // Pick up 5 pixels around center point
            void ReadSurroundingPixels(uint16* pixelValues, Point& centerPoint)
            {
                pixelValues[0] = image->GetPixel(centerPoint);
                pixelValues[1] = image->GetPixel(centerPoint.LeftNeighbor());
                pixelValues[2] = image->GetPixel(centerPoint.UpNeighbor());
                pixelValues[3] = image->GetPixel(centerPoint.RightNeighbor());
                pixelValues[4] = image->GetPixel(centerPoint.DownNeighbor());
            }

            void TestPixelOnLeft(uint16* pixelValues, Point& point, uint16 areaCode, deque<Point>& growingSeeds)
            {
                if (areaMap->GetPixel(point) == 0)
                {
                    // TODO: Current implementation is a hard threshold based. Need improvement
                    // TODO: In the future, we should consider the partial differential segmentation on a 5x5 patch
                    if (abs(pixelValues[0] - pixelValues[1]) < borderThreshold)
                    {
                        areaMap->SetPixel(point, areaCode);
                        growingSeeds.push_back(point);
                    }
                }
            }

            void TestPixelOnTop(uint16* pixelValues, Point& point, uint16 areaCode, deque<Point>& growingSeeds)
            {
                if (areaMap->GetPixel(point) == 0)
                {
                    if (abs(pixelValues[0] - pixelValues[2]) < borderThreshold)
                    {
                        areaMap->SetPixel(point, areaCode);
                        growingSeeds.push_back(point);
                    }
                }
            }

            void TestPixelOnRight(uint16* pixelValues, Point& point, uint16 areaCode, deque<Point>& growingSeeds)
            {
                if (areaMap->GetPixel(point) == 0)
                {
                    if (abs(pixelValues[0] - pixelValues[3]) < borderThreshold)
                    {
                        areaMap->SetPixel(point, areaCode);
                        growingSeeds.push_back(point);
                    }
                }
            }

            void TestPixelOnBottom(uint16* pixelValues, Point& point, uint16 areaCode, deque<Point>& growingSeeds)
            {
                if (areaMap->GetPixel(point) == 0)
                {
                    if (abs(pixelValues[0] - pixelValues[4]) < borderThreshold)
                    {
                        areaMap->SetPixel(point, areaCode);
                        growingSeeds.push_back(point);
                    }
                }
            }

        private:
            deque<Point> seeds;
            shared_ptr<DepthImage> image;
            shared_ptr<AreaMap> areaMap;


            const int borderThreshold;

        };
    }
}