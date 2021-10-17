#ifndef WINRTGRAPHICSCAPTURE_H
#define WINRTGRAPHICSCAPTURE_H
#include <Unknwn.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.h>
#include <winrt/Windows.Graphics.Capture.h>
#include <winrt/Windows.Graphics.DirectX.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Composition.Desktop.h> 
#include <d3d11.h>
#include <dxgi1_2.h>
#include <inspectable.h>
#include <Windows.h>
#include <memory>

namespace WinrtGraphics
{
    enum class InitializerResult
    {
        Success = 0,
        WindowsGraphicsCaptureUnsupported,
        DispatcherQueueInitializationFailed
    };

    InitializerResult InitializeGraphicsCaptureApplication(bool dpiAware = false);

    struct CaptureSource // Represents a source from which you're capturing graphics
    {
        using FrameArrivedHandler = winrt::Windows::Foundation::TypedEventHandler<
            winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool, winrt::Windows::Foundation::IInspectable>;

        using SourceWindowClosedHandler = winrt::Windows::Foundation::TypedEventHandler<
            winrt::Windows::Graphics::Capture::GraphicsCaptureItem, winrt::Windows::Foundation::IInspectable>;

        CaptureSource(FrameArrivedHandler frameArrivedHandler, SourceWindowClosedHandler sourceWindowClosedHandler,
            const winrt::Windows::Graphics::Capture::GraphicsCaptureItem & captureItem,
            winrt::Windows::Graphics::DirectX::DirectXPixelFormat pixelFormat = winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
            int32_t bufferCount = 2);

        ~CaptureSource();

        void recreateFramePool(const winrt::Windows::Graphics::SizeInt32 & size,
            const winrt::Windows::Graphics::DirectX::DirectXPixelFormat & pixelFormat
                = winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
            int32_t bufferCount = 2);

        static auto GetD3dDeviceInterface();

    private:
        winrt::Windows::Graphics::Capture::GraphicsCaptureItem captureItem{ nullptr }; // Source of the graphics
        winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool captureFramePool{ nullptr }; // Stores frames captured by the application
        winrt::Windows::Graphics::Capture::GraphicsCaptureSession captureSession{ nullptr }; // Allows the application to capture graphics
        winrt::Windows::Graphics::Capture::GraphicsCaptureItem::Closed_revoker itemClosedRevoker; // Used to auto close captures when the source window closes

        static inline winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice d3dDevice{ nullptr }; // Interops between windows runtime components
    };
    
    struct Mirror
    {
        enum class CreateResult
        {
            Success = 0,
            InvalidWindow
        };

        // If the source is changed to the parent of the passed sourceWindow, the parent handle is returned, else sourceWindow is returned
        HWND createMirror(HWND hParent, HWND sourceWindow,
            winrt::Windows::Graphics::DirectX::DirectXPixelFormat pixelFormat = winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
            UINT bufferCount = 2);

        void handleSourceSizeChange(LONG sourceWidth, LONG sourceHeight);

        void setClip(LONG left, LONG top, LONG right, LONG bottom, LONG sourceWidth, LONG sourceHeight);

        void clearClip();

        void reset();

    private:

        void createMirrorCasing();

        void copyFrame(const winrt::Windows::Graphics::Capture::Direct3D11CaptureFrame & frame);

        void renderFrame(const winrt::Windows::Graphics::Capture::Direct3D11CaptureFrame & frame);

        void frameArrived(const winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool & framePool, const winrt::Windows::Foundation::IInspectable &);

        void sourceClosed(const winrt::Windows::Graphics::Capture::GraphicsCaptureItem &, const winrt::Windows::Foundation::IInspectable &);

        void refreshSwapChain(bool sizeChanged);

        bool updateClip(bool sizeChanged);

        bool updateSize(const winrt::Windows::Graphics::SizeInt32 & size);

        bool updatePixelFormat(const winrt::Windows::Graphics::DirectX::DirectXPixelFormat & pixelFormat);

        HWND hParent = NULL;
        HWND hSource = NULL;
        std::unique_ptr<CaptureSource> graphicsCaptureSource;

        winrt::com_ptr<ID3D11DeviceContext> d3dContext{ nullptr }; // A device which generates rendering commands (used to copy between buffers)
        winrt::com_ptr<IDXGISwapChain1> swapChain{ nullptr }; // Stores rendered data before presenting to output (contains render output method)
        winrt::Windows::Graphics::SizeInt32 size{}; // Most recently cached width and height of the capture source
        winrt::Windows::Graphics::DirectX::DirectXPixelFormat pixelFormat = winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized;
        UINT bufferCount = 0; // Count of buffers in the framePool and swap chain
        
        bool clipped = false; // Whether clipping is enabled
        bool frameClipRegionInvalid = true; // If true, frameClipRegion needs to be recalculated prior to use
        D3D11_BOX frameClipRegion {}; // Clip region applied to graphics capture frames (frame sizes/frame coordinates don't match GDI window sizes/coordinates)
        RECT gdiClipRegion {}; // Source window clip region in windows GDI coordinates
        SIZE gdiSourceSize {}; // Source window size in windows GDI coordinates

        winrt::Windows::UI::Composition::Compositor compositor{ nullptr }; // Creates and manages visual elements associated with the mirror casing
        winrt::Windows::UI::Composition::ContainerVisual containerVisual{ nullptr }; // A node in a visual tree that can have children
        winrt::Windows::UI::Composition::SpriteVisual spriteVisual{ nullptr }; // Hosts visual content, namely compositionSurfaceBrush
        winrt::Windows::UI::Composition::CompositionSurfaceBrush compositionSurfaceBrush{ nullptr }; // Paints spriteVisual with pixels from swapChain surface
        winrt::Windows::UI::Composition::Desktop::DesktopWindowTarget desktopWindowTarget{ nullptr }; // Hooks our parent window to the mirror composition
    };
}

#endif