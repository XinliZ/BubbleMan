#pragma once

#include "Image.h"
#include "Point.h"

namespace KinectVisionLib{
    namespace Core{

        class AreaMap : public Image<uint16>
        {
        public:
            AreaMap(int width, int height)
                : Image<uint16>(width, height)
            {
                this->boundingBoxes.push_back(Rect(0, 0, 0, 0));       // The invalid place holder to fill up [0]
            }
            ~AreaMap()
            {}

            void SetAreaCode(Point point, uint16 areaCode)
            {
                this->boundingBoxes[areaCode] = this->boundingBoxes[areaCode].Extend(point);
                SetPixel(point, areaCode);
            }

            // TODO: could we exceed the value range of uint16 for the area code?
            uint16 CreateNewAreaCode(Point initialPoint)
            {
                this->boundingBoxes.push_back(Rect(initialPoint, Size(1, 1)));
                return maxAreaCode++;
            }

            void ReplaceAreaCode(uint16 areaCode, uint16 newCode)
            {
                for (int i = 0; i < GetHeight(); i++)
                {
                    uint16* scan = GetScan0() + i * GetStride();
                    for (int j = 0; j < GetWidth(); j++)
                    {
                        if (*scan == areaCode) *scan = newCode;

                        scan++;
                    }
                }
            }

            bool IsOpen(Point point) const
            {
                return GetPixel(point) == 0;
            }

            virtual void ToDisplay(uint8* buffer) const
            {
                COLORREF colors[] = { RGB(0, 0, 0), RGB(255, 0, 0), RGB(0, 255, 0), RGB(0, 0, 255), RGB(127, 0, 0), RGB(0, 127, 0), RGB(0, 0, 127), RGB(255, 255, 0), RGB(0, 255, 255), RGB(255, 0, 255), RGB(127, 127, 0), RGB(127, 0, 127), RGB(0, 127, 127) };
                for (int i = 0; i < GetHeight(); i++)
                {
                    const uint16* scan0 = GetScan0() + i * GetStride();
                    for (int j = 0; j < GetWidth(); j++)
                    {
                        auto color = colors[*scan0 % ARRAYSIZE(colors)];
                        buffer[0] = GetRValue(color);
                        buffer[1] = GetGValue(color);
                        buffer[2] = GetBValue(color);
                        buffer[3] = 0xFF;

                        buffer += 4;
                        scan0++;
                    }
                }
            }

            const uint16 GetAreaCount() const { return maxAreaCode; }
            const Rect& GetRect(uint16 areaCode) const
            {
                return boundingBoxes[areaCode];
            }
            

        private:


            bool IsValidAreaCode(uint16 areaCode) const
            {
                return areaCode >= 0 && areaCode < maxAreaCode;
            }


        private:
            uint16 maxAreaCode = 1;        // Leave 0 as invalid value
            vector<Rect> boundingBoxes;
        };
    }
}

