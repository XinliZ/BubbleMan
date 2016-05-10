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
    m_coreInput(nullptr)
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
    this->kinectManager->Initialize(this->canvas);

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


void KinectVision::DirectXPage::canvas_Draw(Microsoft::Graphics::Canvas::UI::Xaml::ICanvasAnimatedControl^ sender, Microsoft::Graphics::Canvas::UI::Xaml::CanvasAnimatedDrawEventArgs^ args)
{
    args->DrawingSession->DrawEllipse(centerX, centerY, radiusX, radiusY, Windows::UI::Colors::Red);
    radiusX--;
    radiusY--;

    kinectManager->RenderView(args->DrawingSession);
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
    
    this->kinectManager->ProcessImage(dX, dY, dZ, dA, dB, dR);
}
