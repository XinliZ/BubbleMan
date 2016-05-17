//
// DirectXPage.xaml.cpp
// Implementation of the DirectXPage class.
//

#include "pch.h"
#include "DirectXPage.xaml.h"

using namespace KinectVision;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::System::Threading;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace concurrency;

DirectXPage::DirectXPage():
    m_windowVisible(true),
    m_coreInput(nullptr),
    pointerDownX(-1.0f),
    pointerDownY(-1.0f)
{
    InitializeComponent();

    // Register event handlers for page lifecycle.
    CoreWindow^ window = Window::Current->CoreWindow;

    window->VisibilityChanged +=
        ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &DirectXPage::OnVisibilityChanged);

    DisplayInformation^ currentDisplayInformation = DisplayInformation::GetForCurrentView();

    currentDisplayInformation->DpiChanged +=
        ref new TypedEventHandler<DisplayInformation^, Object^>(this, &DirectXPage::OnDpiChanged);

    currentDisplayInformation->OrientationChanged +=
        ref new TypedEventHandler<DisplayInformation^, Object^>(this, &DirectXPage::OnOrientationChanged);

    DisplayInformation::DisplayContentsInvalidated +=
        ref new TypedEventHandler<DisplayInformation^, Object^>(this, &DirectXPage::OnDisplayContentsInvalidated);

    swapChainPanel->CompositionScaleChanged += 
        ref new TypedEventHandler<SwapChainPanel^, Object^>(this, &DirectXPage::OnCompositionScaleChanged);

    swapChainPanel->SizeChanged +=
        ref new SizeChangedEventHandler(this, &DirectXPage::OnSwapChainPanelSizeChanged);

    // Disable all pointer visual feedback for better performance when touching.
    auto pointerVisualizationSettings = PointerVisualizationSettings::GetForCurrentView();
    pointerVisualizationSettings->IsContactFeedbackEnabled = false; 
    pointerVisualizationSettings->IsBarrelButtonFeedbackEnabled = false;

    // At this point we have access to the device. 
    // We can create the device-dependent resources.
    m_deviceResources = std::make_shared<DX::DeviceResources>();
    m_deviceResources->SetSwapChainPanel(swapChainPanel);

    // Register our SwapChainPanel to get independent input pointer events
    auto workItemHandler = ref new WorkItemHandler([this] (IAsyncAction ^)
    {
        // The CoreIndependentInputSource will raise pointer events for the specified device types on whichever thread it's created on.
        m_coreInput = swapChainPanel->CreateCoreIndependentInputSource(
            Windows::UI::Core::CoreInputDeviceTypes::Mouse |
            Windows::UI::Core::CoreInputDeviceTypes::Touch |
            Windows::UI::Core::CoreInputDeviceTypes::Pen
            );

        // Register for pointer events, which will be raised on the background thread.
        m_coreInput->PointerPressed += ref new TypedEventHandler<Object^, PointerEventArgs^>(this, &DirectXPage::OnPointerPressed);
        m_coreInput->PointerMoved += ref new TypedEventHandler<Object^, PointerEventArgs^>(this, &DirectXPage::OnPointerMoved);
        m_coreInput->PointerReleased += ref new TypedEventHandler<Object^, PointerEventArgs^>(this, &DirectXPage::OnPointerReleased);

        // Begin processing input messages as they're delivered.
        m_coreInput->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessUntilQuit);
    });

    // Run task on a dedicated high priority background thread.
    m_inputLoopWorker = ThreadPool::RunAsync(workItemHandler, WorkItemPriority::High, WorkItemOptions::TimeSliced);

    this->kinectManager = ref new KinectManager();
    this->kinectManager->Initialize(this->canvasTop);
    this->kinectManager->ErrorStatsUpdated += ref new KinectVision::ErrorStatsUpdatedEventHandler([this](KinectVisionLib::ErrorStats^ error) {
        this->meanSquareError->Text = error->GetMeanSquareError().ToString();
        this->positiveError->Text = error->GetPositiveError().ToString();
        this->negativeError->Text = error->GetNegativeError().ToString();
        this->xOffset->Text = error->GetXOffset().ToString();
        this->yOffset->Text = error->GetYOffset().ToString();
    });

    // Wire up the display window to the frame sources
    this->kinectManager->NormalYFrameUpdated += ref new KinectVision::FrameUpdatedEventHandler(this, &DirectXPage::UpdateTopFrame);
    this->kinectManager->NormalXFrameUpdated += ref new KinectVision::FrameUpdatedEventHandler(this, &DirectXPage::UpdateBottomFrame);

    m_main = std::unique_ptr<KinectVisionMain>(new KinectVisionMain(m_deviceResources));
    m_main->StartRenderLoop(this->kinectManager);
}

DirectXPage::~DirectXPage()
{
    // Stop rendering and processing events on destruction.
    m_main->StopRenderLoop();
    m_coreInput->Dispatcher->StopProcessEvents();
}

// Saves the current state of the app for suspend and terminate events.
void DirectXPage::SaveInternalState(IPropertySet^ state)
{
    critical_section::scoped_lock lock(m_main->GetCriticalSection());
    m_deviceResources->Trim();

    // Stop rendering when the app is suspended.
    m_main->StopRenderLoop();

    // Put code to save app state here.
}

// Loads the current state of the app for resume events.
void DirectXPage::LoadInternalState(IPropertySet^ state)
{
    // Put code to load app state here.

    // Start rendering when the app is resumed.
    m_main->StartRenderLoop(this->kinectManager);
}

// Window event handlers.

void DirectXPage::OnVisibilityChanged(CoreWindow^ sender, VisibilityChangedEventArgs^ args)
{
    m_windowVisible = args->Visible;
    if (m_windowVisible)
    {
        m_main->StartRenderLoop(this->kinectManager);
    }
    else
    {
        m_main->StopRenderLoop();
    }
}

// DisplayInformation event handlers.

void DirectXPage::OnDpiChanged(DisplayInformation^ sender, Object^ args)
{
    critical_section::scoped_lock lock(m_main->GetCriticalSection());
    m_deviceResources->SetDpi(sender->LogicalDpi);
    m_main->CreateWindowSizeDependentResources();
}

void DirectXPage::OnOrientationChanged(DisplayInformation^ sender, Object^ args)
{
    critical_section::scoped_lock lock(m_main->GetCriticalSection());
    m_deviceResources->SetCurrentOrientation(sender->CurrentOrientation);
    m_main->CreateWindowSizeDependentResources();
}


void DirectXPage::OnDisplayContentsInvalidated(DisplayInformation^ sender, Object^ args)
{
    critical_section::scoped_lock lock(m_main->GetCriticalSection());
    m_deviceResources->ValidateDevice();
}

// Called when the app bar button is clicked.
void DirectXPage::AppBarButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    // Use the app bar if it is appropriate for your app. Design the app bar, 
    // then fill in event handlers (like this one).
}

void DirectXPage::OnPointerPressed(Object^ sender, PointerEventArgs^ e)
{
    // When the pointer is pressed begin tracking the pointer movement.
    m_main->StartTracking();
    this->mouseXPosition = e->CurrentPoint->Position.X;
}

void DirectXPage::OnPointerMoved(Object^ sender, PointerEventArgs^ e)
{
    // Update the pointer tracking code.
    if (m_main->IsTracking())
    {
        m_main->TrackingUpdate(e->CurrentPoint->Position.X - this->mouseXPosition);
    }
}

void DirectXPage::OnPointerReleased(Object^ sender, PointerEventArgs^ e)
{
    // Stop tracking pointer movement when the pointer is released.
    m_main->StopTracking();
}

void DirectXPage::OnCompositionScaleChanged(SwapChainPanel^ sender, Object^ args)
{
    critical_section::scoped_lock lock(m_main->GetCriticalSection());
    m_deviceResources->SetCompositionScale(sender->CompositionScaleX, sender->CompositionScaleY);
    m_main->CreateWindowSizeDependentResources();
}

void DirectXPage::OnSwapChainPanelSizeChanged(Object^ sender, SizeChangedEventArgs^ e)
{
    critical_section::scoped_lock lock(m_main->GetCriticalSection());
    m_deviceResources->SetLogicalSize(e->NewSize);
    m_main->CreateWindowSizeDependentResources();
}

void KinectVision::DirectXPage::Page_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    this->centerX = 200;
    this->centerY = 200;
    this->radiusX = 200;
    this->radiusY = 300;
}

void KinectVision::DirectXPage::Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    this->kinectManager->ConnectToKinect();
}


void KinectVision::DirectXPage::Button_Click_1(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    this->kinectManager->DisconnectKinect();
    this->kinectManager->LoadFolder();
}

void KinectVision::DirectXPage::Button_Click_3(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    this->kinectManager->LoadNextFrame();
}


void KinectVision::DirectXPage::CheckBox_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    bool enabled = ((CheckBox^)sender)->IsChecked->Value;
    critical_section::scoped_lock lock(m_main->GetCriticalSection());
    this->m_main->EnableMeshRendering(enabled);
}


void KinectVision::DirectXPage::Button_Click_2(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // Toolbar button
    if (this->ToolsPanel->Height == 0)
    {
        this->ToolsPanel->Height = nan("");
    }
    else
    {
        this->ToolsPanel->Height = 0;
    }
}


void KinectVision::DirectXPage::ProcessDepthImage_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    float dX = wcstof(this->dX->Text->Data(), nullptr);
    float dY = wcstof(this->dY->Text->Data(), nullptr);
    float dZ = wcstof(this->dZ->Text->Data(), nullptr);
    float dA = wcstof(this->dA->Text->Data(), nullptr);
    float dB = wcstof(this->dB->Text->Data(), nullptr);
    float dR = wcstof(this->dR->Text->Data(), nullptr);
    
    if (!this->kinectManager->ProcessImage(dX, dY, dZ, dA, dB, dR))
    {
        auto dialog = ref new Windows::UI::Popups::MessageDialog("Not able to process the command. We need at least 2 frames to process.");
        dialog->Commands->Append(ref new Windows::UI::Popups::UICommand("Ok"));
        dialog->ShowAsync();
    }
}


void KinectVision::DirectXPage::canvas_DrawTop(Microsoft::Graphics::Canvas::UI::Xaml::ICanvasAnimatedControl^ sender, Microsoft::Graphics::Canvas::UI::Xaml::CanvasAnimatedDrawEventArgs^ args)
{
    args->DrawingSession->DrawEllipse(centerX, centerY, radiusX, radiusY, Windows::UI::Colors::Red);
    radiusX--;
    radiusY--;

    Microsoft::Graphics::Canvas::ICanvasImage^ volatile bitmap = this->bitmapTop.GetData();
    if (bitmap != nullptr)
    {
        args->DrawingSession->DrawImage(bitmap);
    }
}


void KinectVision::DirectXPage::canvas_DrawBottom(Microsoft::Graphics::Canvas::UI::Xaml::ICanvasAnimatedControl^ sender, Microsoft::Graphics::Canvas::UI::Xaml::CanvasAnimatedDrawEventArgs^ args)
{
    Microsoft::Graphics::Canvas::ICanvasImage^ volatile bitmap = this->bitmapBottom.GetData();
    if (bitmap != nullptr)
    {
        args->DrawingSession->DrawImage(bitmap);
    }
}

void KinectVision::DirectXPage::UpdateTopFrame(KinectVisionLib::Frame^ frame)
{
    this->frameTop = frame;
    this->bitmapTop.SetData(frame->GetBitmap(this->canvasTop));
}
void KinectVision::DirectXPage::UpdateBottomFrame(KinectVisionLib::Frame^ frame)
{
    this->frameBottom = frame;
    this->bitmapBottom.SetData(frame->GetBitmap(this->canvasBottom));
}


void KinectVision::DirectXPage::Grid_PointerMoved(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
    Grid^ grid = (Grid^)sender;
    auto position = e->GetCurrentPoint(grid)->Position;
    auto targetFrame = Platform::String::CompareOrdinal(grid->Name, L"gridTop") == 0 ? this->frameTop : this->frameBottom;
    if (targetFrame == nullptr)
    {
        return;
    }

    if (this->pointerDownX == -1.0f)
    {
        this->mouseXLocation->Text = position.X.ToString();
        this->mouseYLocation->Text = position.Y.ToString();
        
        this->readValue->Text = targetFrame->ReadPixelValue((int)position.X, (int)position.Y, 1, 1);
    }
    else
    {
        auto x = this->pointerDownX;
        auto y = this->pointerDownY;
        auto w = position.X - this->pointerDownX;
        auto h = position.Y - this->pointerDownY;
        if (w < 0)
        {
            x = x + w;
            w = -w;
        }
        if (h < 0)
        {
            y = y + h;
            h = -h;
        }
        this->mouseXLocation->Text = x.ToString();
        this->mouseYLocation->Text = y.ToString();
        this->mouseWidth->Text = w.ToString();
        this->mouseHeight->Text = h.ToString();
        this->readValue->Text = targetFrame->ReadPixelValue((int)x, (int)y, (int)w, (int)h);

        this->rectangleOnImage->Width = w;
        this->rectangleOnImage->Height = h;
        this->rectangleOnImage->Margin = Thickness(x, y, 0, 0);
    }
}


void KinectVision::DirectXPage::Grid_PointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
    auto position = e->GetCurrentPoint((UIElement^)sender)->Position;
    this->pointerDownX = position.X;
    this->pointerDownY = position.Y;

    if (this->rectangleOnImage->Parent != sender)
    {
        ((Grid^)this->rectangleOnImage->Parent)->Children->RemoveAt(1);
        ((Grid^)sender)->Children->Append(this->rectangleOnImage);
    }
    this->rectangleOnImage->Visibility = Windows::UI::Xaml::Visibility::Visible;
}


void KinectVision::DirectXPage::Grid_PointerReleased(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
    this->pointerDownX = -1.0f;
    this->pointerDownY = -1.0f;
    this->mouseWidth->Text = L"";
    this->mouseHeight->Text = L"";

    this->rectangleOnImage->Width = 0;
    this->rectangleOnImage->Height = 0;
    this->rectangleOnImage->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
}


void KinectVision::DirectXPage::gridTop_PointerExited(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
    this->pointerDownX = -1.0f;
    this->pointerDownY = -1.0f;
    this->mouseXLocation->Text = L"";
    this->mouseYLocation->Text = L"";
    this->mouseWidth->Text = L"";
    this->mouseHeight->Text = L"";
    this->readValue->Text = L"";

    this->rectangleOnImage->Width = 0;
    this->rectangleOnImage->Height = 0;
    this->rectangleOnImage->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
}
