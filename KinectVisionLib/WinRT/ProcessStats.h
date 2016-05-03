#pragma once

#include "Frame.h"

namespace KinectVisionLib
{
    public ref class ProcessStats sealed
    {
    public:
        ProcessStats(Frame^ resultFrame);

        void AddDebugFrame(Platform::String^ name, Frame^ debugFrame);

        Frame^ GetDebugFrame(Platform::String^ name);

    private:
        int processTime;
        std::map<Platform::String^, Frame^> debugFrames;
        Frame^ resultFrame;
    };
}
