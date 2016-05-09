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

            virtual void ToDisplay(uint8* buffer) const override {}

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
        };
    }
}