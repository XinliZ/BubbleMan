#pragma once

#include "ProcessStats.h"
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

    private:
        KinectVisionLib::Core::KinectVisionManager manager;
    };
}
