#include "pch.h"
#include "KinectManager.h"

using namespace concurrency;
using namespace Platform;
using namespace Microsoft::Graphics::Canvas;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace WindowsPreview::Kinect;

using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage::Streams;

using namespace KinectVision;

KinectManager::KinectManager()
{
    this->kinectVision = ref new KinectVisionLib::KinectVision();
    this->kinectVision->Initialize();
}

void KinectManager::Initialize(ICanvasResourceCreator^ canvasResourceCreator)
{
    this->canvasResourceCreator = canvasResourceCreator;
    this->kinectSensor = KinectSensor::GetDefault();
}

void KinectManager::ConnectToKinect()
{
    kinectSensor->Open();

    this->multiSourceFrameReader = kinectSensor->OpenMultiSourceFrameReader(
        FrameSourceTypes::Depth |
        FrameSourceTypes::Color |
        FrameSourceTypes::Infrared);

    if (this->multiSourceFrameReader != nullptr)
    {
        this->multiSourceFrameReader->MultiSourceFrameArrived += ref new TypedEventHandler<MultiSourceFrameReader ^, MultiSourceFrameArrivedEventArgs ^>(this, &KinectManager::OnMultiSourceFrameArrived);
    }
    else
    {
        throw ref new Platform::NullReferenceException(L"Failed to open MultiSourceFrameReader.");
    }
}

void KinectManager::DisconnectKinect()
{
    // TODO: Need to dispose this reader?
    //this->multiSourceFrameReader->Close();
    this->multiSourceFrameReader = nullptr;

    this->kinectSensor->Close();
}

void KinectManager::LoadFolder()
{
    auto openPicker = ref new FolderPicker();
    openPicker->FileTypeFilter->Append(".png");
    openPicker->SuggestedStartLocation = PickerLocationId::PicturesLibrary;
    create_task(openPicker->PickSingleFolderAsync()).then([this](StorageFolder^ folder){
        if (folder == nullptr)
        {
            this->fileList = nullptr;
            throw task_canceled();
        }
        return folder->GetFilesAsync();
    }).then([this](IVectorView<StorageFile^>^ fileList){
        this->fileList = fileList;
        this->frameNumber = 0;
        this->LoadNextFrame();
    });
}

void KinectManager::LoadNextFrame()
{
    if (this->frameNumber == this->fileList->Size)
    {
        this->frameNumber = 0;
    }

    bool skippingFiles = true;
    StorageFile^ file;
    while (skippingFiles && this->frameNumber < this->fileList->Size)
    {
        file = this->fileList->GetAt(frameNumber);
        frameNumber++;
        if (String::CompareOrdinal(file->Name, L"ColorImage") < 0 || String::CompareOrdinal(file->Name, L"ColorImageA") >= 0)
        {
            // Only pass the file name doesn't match ColorImage000XX.png
            skippingFiles = false;
        }
    }
    if (file == nullptr)
    {
        throw ref new InvalidArgumentException("The file from list is not valid.");
    }

    create_task(file->OpenAsync(FileAccessMode::Read)).then([this](IRandomAccessStream^ stream){
        return CanvasBitmap::LoadAsync(this->canvasResourceCreator, stream);
    }).then([this](CanvasBitmap^ bitmap){
        //this->canvasBitmap = BuildCanvasBitmapFromBitmap(this->canvasResourceCreator, bitmap);
        ProcessFrame(BuildFrameFromBitmap(bitmap));
    });
}

void KinectManager::RenderView(CanvasDrawingSession^ drawingSession)
{
    // The input
    ICanvasImage^ img0 = this->canvasBitmap0.GetData();
    ICanvasImage^ img1 = this->canvasBitmap1.GetData();
    if (img0 != nullptr)
    {
        drawingSession->DrawImage(img0);
    }
    if (img1 != nullptr)
    {
        drawingSession->DrawImage(img1, 0, 430);
    }
}

void KinectManager::OnMultiSourceFrameArrived(MultiSourceFrameReader ^sender, MultiSourceFrameArrivedEventArgs ^args)
{
    auto multiSourceFrame = sender->AcquireLatestFrame();

    auto depthFrame = multiSourceFrame != nullptr ? multiSourceFrame->DepthFrameReference->AcquireFrame() : nullptr;
    if (depthFrame != nullptr)
    {
        //this->canvasBitmap = BuildCanvasBitmapFromDepthFrame(this->canvasResourceCreator, depthFrame);
        ProcessFrame(BuildFrameFromDepthFrame(depthFrame));
    }

    //auto colorFrame = multiSourceFrame->ColorFrameReference->AcquireFrame();


}

CanvasBitmap^ KinectManager::BuildCanvasBitmapFromBitmap(ICanvasResourceCreator^ canvas, CanvasBitmap^ bitmap)
{
    int width = (int)bitmap->Size.Width;
    int height = (int)bitmap->Size.Height;
    int minDepth = 500;
    int maxDepth = 4500;

    auto sourceBuffer = bitmap->GetPixelBytes();
    auto buffer = ref new Array<uint16>(width * height);

    for (int i = 0; i < width * height; i++)
    {
        auto index = i * 4;
        uint16 value = sourceBuffer[index + 2] | (sourceBuffer[index + 1] << 8);
        buffer[i] = value;
    }

    return CreateBitmapFromBuffer(canvas, buffer, width, height, minDepth, maxDepth);
}

CanvasBitmap^ KinectManager::BuildCanvasBitmapFromDepthFrame(ICanvasResourceCreator^ canvas, DepthFrame^ depthFrame)
{
    auto width = depthFrame->FrameDescription->Width;
    auto height = depthFrame->FrameDescription->Height;
    auto minDepth = depthFrame->DepthMinReliableDistance;
    auto maxDepth = depthFrame->DepthMaxReliableDistance;

    auto buffer = ref new Array<uint16>(depthFrame->FrameDescription->LengthInPixels);
    depthFrame->CopyFrameDataToArray(buffer);

    return CreateBitmapFromBuffer(canvas, buffer, width, height, minDepth, maxDepth);
}

CanvasBitmap^ KinectManager::CreateBitmapFromBuffer(ICanvasResourceCreator^ canvas, Array<uint16>^ buffer, int width, int height, int minDepth, int maxDepth)
{
    auto bitmapBuffer = ref new Array<byte>(width * height * 4);
    NormalizeDepthDataToBuffer(bitmapBuffer, buffer, minDepth, maxDepth);

    return CanvasBitmap::CreateFromBytes(canvas, bitmapBuffer, width, height, Microsoft::Graphics::Canvas::DirectX::DirectXPixelFormat::R8G8B8A8UIntNormalized);

}

void KinectManager::NormalizeDepthDataToBuffer(Array<byte>^ bitmapBuffer, Array<uint16>^ sourceBuffer, uint16 minDepth, uint16 maxDepth)
{
    const uint32 MapDepthToByte = 4500 / 256;
    for (uint32 i = 0; i < sourceBuffer->Length; i++)
    {
        //if (sourceBuffer[i] == 0)
        //{
        //    bitmapBuffer[i * 4 + 1] = 0xFF;
        //    bitmapBuffer[i * 4 + 3] = 0xFF;
        //}
        //else if (sourceBuffer[i] <= minDepth)
        //{
        //    bitmapBuffer[i * 4] = 0xFF;
        //    bitmapBuffer[i * 4 + 3] = 0xFF;
        //}
        //else if (sourceBuffer[i] > maxDepth)
        //{
        //    bitmapBuffer[i * 4 + 2] = 0xFF;
        //    bitmapBuffer[i * 4 + 3] = 0xFF;
        //}
        //else
        {
            byte depth = (byte)(sourceBuffer[i] / MapDepthToByte);
            bitmapBuffer[i * 4] = depth;
            bitmapBuffer[i * 4 + 1] = depth;
            bitmapBuffer[i * 4 + 2] = depth;
            bitmapBuffer[i * 4 + 3] = 0xFF;
        }
    }
}

KinectVisionLib::Frame^ KinectManager::BuildFrameFromBitmap(CanvasBitmap^ bitmap)
{
    int width = (int)bitmap->Size.Width;
    int height = (int)bitmap->Size.Height;

    auto sourceBuffer = bitmap->GetPixelBytes();
    auto buffer = ref new Array<uint16>(width * height);

    for (int i = 0; i < width * height; i++)
    {
        auto index = i * 4;
        uint16 value = sourceBuffer[index + 2] | (sourceBuffer[index + 1] << 8);
        buffer[i] = value;
    }

    return ref new KinectVisionLib::Frame(buffer, width, height);
}

KinectVisionLib::Frame^ KinectManager::BuildFrameFromDepthFrame(DepthFrame^ depthFrame)
{
    auto width = depthFrame->FrameDescription->Width;
    auto height = depthFrame->FrameDescription->Height;

    auto buffer = ref new Array<uint16>(depthFrame->FrameDescription->LengthInPixels);
    depthFrame->CopyFrameDataToArray(buffer);

    return ref new KinectVisionLib::Frame(buffer, width, height);
}

void KinectManager::ProcessFrame(KinectVisionLib::Frame^ frame)
{
    bool DoNotProcessForDebugging = true;
    if (DoNotProcessForDebugging)
    {
        this->previousFrame = this->currentFrame;
        this->currentFrame = frame;
        create_task(kinectVision->GetXNormalFrame(frame)).then([this](KinectVisionLib::Frame^ result) {
            this->canvasBitmap0.SetData(result->GetBitmap(this->canvasResourceCreator));
        });
    }
    else
    {
        create_task(kinectVision->ProcessFrame(frame)).then([this, frame](KinectVisionLib::ProcessStats^ stats){
            this->previousFrame = this->currentFrame;
            this->currentFrame = frame;
            this->canvasBitmap0.SetData(stats->GetDebugFrame(nullptr)->GetBitmap(this->canvasResourceCreator));
            auto frame1 = stats->GetDebugFrame(L"BackgroundDiff");
            if (frame1 != nullptr)
            {
                this->canvasBitmap1.SetData(frame1->GetBitmap(this->canvasResourceCreator));
            }
        });
    }
}

bool KinectManager::ProcessImage(float dX, float dY, float dZ, float dA, float dB, float dR)
{
    if (this->currentFrame != nullptr && this->previousFrame != nullptr)
    {
        ProcessImage(this->currentFrame, this->previousFrame, dX, dY, dZ, dA, dB, dR, 0);
        return true;
    }
    return false;
}

void KinectManager::ProcessImage(KinectVisionLib::Frame^ currentFrame, KinectVisionLib::Frame^ previousFrame, float dX, float dY, float dZ, float dA, float dB, float dR, int iteration)
{
    create_task(kinectVision->TransformFrame(this->currentFrame, this->previousFrame, dX, dY, dZ, dA, dB, dR))
        .then([this, iteration, currentFrame, previousFrame](KinectVisionLib::ErrorStats^ result) {
            // TODO: Result should be score and error map
            this->canvasBitmap1.SetData(result->GetErrorFrame()->GetBitmap(this->canvasResourceCreator));
            ErrorStatsUpdated(result);

            if (iteration < 10 && result != nullptr /* Check the error score */)
            {
                //ProcessImage(currentFrame, previousFrame, dX, dY, dZ, dA, dB, dR, iteration + 1);
            }
        });
}