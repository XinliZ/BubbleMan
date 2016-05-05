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

            float GetScore() const
            {
                // TODO: Implementation needed
                return 0.0f;
            }

            virtual void ToDisplay(uint8* buffer) const override {}
        };
    }
}