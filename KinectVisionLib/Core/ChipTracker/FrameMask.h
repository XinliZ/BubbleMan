#pragma once

#include "../LocalImage.h"

namespace KinectVisionLib
{
    namespace Core
    {
        class FrameMask : public LocalImage<bool>
        {
        public:
            FrameMask(shared_ptr<const AreaMap> areaMap, uint16 areaCode)
                : LocalImage<bool>(areaMap->GetRect(areaCode))
            {
                ImageOperation<uint16>(areaMap.get(), areaMap->GetRect(areaCode), [areaCode](bool* pixel, const uint16* source) {
                    *pixel = (*source == areaCode) ? true : false;
                });
            }

            virtual void ToDisplay(uint8* buffer) const
            {
                // Do nothing, we don't need this for now
            }

            bool IsPixelValid(Point point)
            {
                return GetBoundingBox().Contains(point) ? GetPixel(point.Offset(GetBoundingBox().GetOffset())) : false;
            }
        };

    }
}

