#pragma once

#include "Frame.h"

namespace KinectVisionLib
{
    public ref class ProcessStats sealed
    {
    public:
        ProcessStats(Frame^ debugFrame);

        Frame^ GetDebugFrame();

    private:
        int processTime;
        Frame^ debugFrame;
    };
}
