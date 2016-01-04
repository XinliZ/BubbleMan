#include "pch.h"
#include "ProcessStats.h"

using namespace KinectVisionLib;

ProcessStats::ProcessStats(Frame^ debugFrame)
{
	this->debugFrame = debugFrame;
}

Frame^ ProcessStats::GetDebugFrame()
{
	return this->debugFrame;
}