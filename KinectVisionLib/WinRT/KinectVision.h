#pragma once

#include "ProcessStats.h"
#include "ErrorStats.h"
#include "Frame.h"
#include "Core/KinectVisionManager.h"

namespace KinectVisionLib
{
    public ref class KinectVision sealed
    {
    public:
        KinectVision();

        void Initialize();

        Windows::Foundation::IAsyncOperation<ProcessStats^>^ ProcessFrame(Frame^ frame);

        Windows::Foundation::IAsyncOperation<ErrorStats^>^ TransformFrame(Frame^ frame, Frame^ previousFrame, float dX, float dY, float dZ, float dA, float dB, float dR);

    private:
        KinectVisionLib::Core::KinectVisionManager manager;
    };
}
