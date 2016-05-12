#pragma once

#include "Frame.h"

namespace KinectVisionLib
{
    public ref class ErrorStats sealed
    {
    public:
        ErrorStats(Frame^ errorFrame, float meanSquareError, float positiveError, float negativeError)
        {
            this->errorFrame = errorFrame;
            this->meanSquareError = meanSquareError;
            this->positiveError = positiveError;
            this->negativeError = negativeError;
        }

        Frame^ GetErrorFrame() { return this->errorFrame; }

        float GetMeanSquareError() { return this->meanSquareError; }
        float GetPositiveError() { return this->positiveError; }
        float GetNegativeError() { return this->negativeError; }

    private:
        Frame^ errorFrame;

        float meanSquareError;
        float positiveError;
        float negativeError;
    };
}