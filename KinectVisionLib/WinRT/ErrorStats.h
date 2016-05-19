#pragma once

#include "Frame.h"

namespace KinectVisionLib
{
    public ref class ErrorStats sealed
    {
    public:
        ErrorStats(Frame^ errorFrame, float meanSquareError, float positiveError, float negativeError, 
            float xOffset, float yOffset,
            float iterativeDX, float iterativeDY, float iterativeDZ, float iterativeDA, float iterativeDB, float iterativeDR)
        {
            this->errorFrame = errorFrame;
            this->meanSquareError = meanSquareError;
            this->positiveError = positiveError;
            this->negativeError = negativeError;
            this->xOffset = xOffset;
            this->yOffset = yOffset;

            this->iterativeDX = iterativeDX;
            this->iterativeDY = iterativeDY;
            this->iterativeDZ = iterativeDZ;
            this->iterativeDA = iterativeDA;
            this->iterativeDB = iterativeDB;
            this->iterativeDR = iterativeDR;
        }

        Frame^ GetErrorFrame() { return this->errorFrame; }

        float GetMeanSquareError() { return this->meanSquareError; }
        float GetPositiveError() { return this->positiveError; }
        float GetNegativeError() { return this->negativeError; }
        float GetXOffset() { return this->xOffset; }
        float GetYOffset() { return this->yOffset; }

        float GetDX() { return this->iterativeDX; }
        float GetDY() { return this->iterativeDY; }
        float GetDZ() { return this->iterativeDZ; }
        float GetDA() { return this->iterativeDA; }
        float GetDB() { return this->iterativeDB; }
        float GetDR() { return this->iterativeDR; }

    private:
        Frame^ errorFrame;

        float meanSquareError;
        float positiveError;
        float negativeError;
        float xOffset;
        float yOffset;

        float iterativeDX;
        float iterativeDY;
        float iterativeDZ;
        float iterativeDA;
        float iterativeDB;
        float iterativeDR;
    };
}