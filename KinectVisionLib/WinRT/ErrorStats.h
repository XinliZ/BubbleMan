#pragma once

#include "Frame.h"

namespace KinectVisionLib
{
    public ref class ErrorStats sealed
    {
    public:
        ErrorStats(Frame^ errorFrame, float meanSquareError, float positiveError, float negativeError, float xOffset, float yOffset)
        {
            this->errorFrame = errorFrame;
            this->meanSquareError = meanSquareError;
            this->positiveError = positiveError;
            this->negativeError = negativeError;
            this->xOffset = xOffset;
            this->yOffset = yOffset;
        }

        Frame^ GetErrorFrame() { return this->errorFrame; }

        float GetMeanSquareError() { return this->meanSquareError; }
        float GetPositiveError() { return this->positiveError; }
        float GetNegativeError() { return this->negativeError; }
        float GetXOffset() { return this->xOffset; }
        float GetYOffset() { return this->yOffset; }

    private:
        Frame^ errorFrame;

        float meanSquareError;
        float positiveError;
        float negativeError;
        float xOffset;
        float yOffset;
    };
}