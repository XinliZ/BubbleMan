#pragma once

#include "Frame.h"

namespace KinectVisionLib
{
    public ref class ProcessStats sealed
    {
    public:
        ProcessStats(Frame^ debugFrame, Frame^ depthFrame)
        {
            this->debugFrame = debugFrame;
            this->depthFrame = depthFrame;
        }

        Frame^ GetDebugFrame() { return this->debugFrame; }
        Frame^ GetDepthFrame() { return this->depthFrame; }

    private:
        int processTime;
        Frame^ debugFrame;
        Frame^ depthFrame;
    };
}
