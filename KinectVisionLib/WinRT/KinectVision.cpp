#include "pch.h"
#include "KinectVision.h"
#include "ProcessStats.h"
#include "ErrorStats.h"
#include "Core/Image.h"
#include "Core/KinectVisionManager.h"

using namespace KinectVisionLib;
using namespace Platform;
using namespace concurrency;
using namespace Microsoft::Graphics::Canvas;
using namespace WindowsPreview::Kinect;

KinectVision::KinectVision()
{
}

void KinectVision::Initialize()
{

}

Windows::Foundation::IAsyncOperation<ProcessStats^>^ KinectVision::ProcessFrame(Frame^ frame)
{
    return create_async([this, frame]() -> ProcessStats^ {
        auto result = manager.FeedFrame(frame->GetImage());

        auto stats = ref new ProcessStats(ref new Frame(result));
        for each (auto pair in manager.GetProcessContext()->debugFrames)
        {
            stats->AddDebugFrame(ref new Platform::String(pair.first.c_str()), ref new Frame(pair.second));
        }
        return stats;
    });
}


Windows::Foundation::IAsyncOperation<ErrorStats^>^ KinectVision::TransformFrame(Frame^ frame, Frame^ previousFrame, float dX, float dY, float dZ, float dA, float dB, float dR)
{
    return create_async([=]() -> ErrorStats^ {
        auto errorMap = manager.TransformFrame(frame->GetImage(), previousFrame->GetImage(), dX, dY, dZ, dA, dB, dR);
        return ref new ErrorStats(ref new Frame(errorMap), errorMap->GetMeanSquareError(), errorMap->GetPositiveError(), errorMap->GetNegativeError());
    });
}