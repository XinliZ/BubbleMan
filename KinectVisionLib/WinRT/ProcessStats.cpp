#include "pch.h"
#include "ProcessStats.h"

using namespace KinectVisionLib;

ProcessStats::ProcessStats(Frame^ resultFrame)
{
    this->resultFrame = resultFrame;
}

void ProcessStats::AddDebugFrame(Platform::String^ name, Frame^ debugFrame)
{
    debugFrames.insert(std::pair<Platform::String^, Frame^>(name, debugFrame));
}

Frame^ ProcessStats::GetDebugFrame(Platform::String^ name)
{
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