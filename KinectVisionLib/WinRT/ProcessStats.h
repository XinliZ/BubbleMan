#pragma once

#include "Frame.h"

namespace KinectVisionLib
{
    public ref class ProcessStats sealed
    {
    public:
        ProcessStats(Frame^ resultFrame) {
            this->resultFrame = resultFrame;
        }

        void AddDebugFrame(Platform::String^ name, Frame^ debugFrame) {
            debugFrames.insert(std::pair<Platform::String^, Frame^>(name, debugFrame));
        }

        Frame^ GetDebugFrame(Platform::String^ name) {
            if (name == nullptr)
            {
                return this->resultFrame;
            }
            else
            {
                auto ret = debugFrames.find(name);
                if (ret != debugFrames.end())
                {
                    return ret->second;
                }
                return nullptr;
            }
        }

    private:
        int processTime;
        std::map<Platform::String^, Frame^> debugFrames;
        Frame^ resultFrame;
    };
}
