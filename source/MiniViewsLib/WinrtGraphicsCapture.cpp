#include "WinrtGraphicsCapture.h"
#include <winrt/base.h>
#include <winrt/Windows.System.h>
#include <windows.graphics.capture.interop.h>
#include <windows.graphics.directx.direct3d11.h>
#include <windows.graphics.directx.direct3d11.interop.h>
#include <windows.ui.composition.interop.h>
#include <d3dcommon.h>
#include <dispatcherqueue.h>
#include <dxgiformat.h>
#include <dxgi.h>
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <string>

namespace WinrtGraphics
{
    struct Initializer
    {
        static inline thread_local winrt::Windows::System::DispatcherQueueController dispatcherQueueController{ nullptr };
        static inline thread_local winrt::Windows::System::DispatcherQueue mainDispatcherQueue{ nullptr };
    };

    InitializerResult InitializeGraphicsCaptureApplication(bool dpiAware)
    {
        if ( dpiAware )
            SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

        winrt::init_apartment(); // Initialize COM and thread in windows runtime in a multithreaded apartment

        if ( !winrt::Windows::Graphics::Capture::GraphicsCaptureSession::IsSupported() )
            return InitializerResult::WindowsGraphicsCaptureUnsupported;

        DispatcherQueueOptions options{};
        options.dwSize = sizeof(DispatcherQueueOptions);
        options.threadType = DQTYPE_THREAD_CURRENT;
        options.apartmentType = DQTAT_COM_NONE;
        winrt::check_hresult(CreateDispatcherQueueController(options, reinterpret_cast<ABI::Windows::System::IDispatcherQueueController**>(
            winrt::put_abi(Initializer::dispatcherQueueController))));

        Initializer::mainDispatcherQueue = winrt::Windows::System::DispatcherQueue::GetForCurrentThread();

        return Initializer::mainDispatcherQueue == nullptr ? InitializerResult::DispatcherQueueInitializationFailed : InitializerResult::Success;
    }

    struct PixelData // Converts graphics capture frames to BGRA pixel data, unused at present, but worth keeping around
    {
        size_t width;
        size_t height;
        std::unique_ptr<std::uint32_t[]> pixels;

        void clear()
        {
            this->width = 0;
            this->height = 0;
            this->pixels = nullptr;
        }

        bool copyFromTexture(winrt::com_ptr<ID3D11Texture2D> & texture)
        {
            D3D11_TEXTURE2D_DESC textureDescriptor{};
            texture->GetDesc(&textureDescriptor);
            size_t width = size_t(textureDescriptor.Width);
            size_t height = size_t(textureDescriptor.Height);
            if ( textureDescriptor.Format != DXGI_FORMAT_B8G8R8A8_UNORM )
                return false;

            winrt::com_ptr<ID3D11Device> d3dDevice{};
            winrt::com_ptr<ID3D11DeviceContext> d3dDeviceContext{};
            D3D11_MAPPED_SUBRESOURCE mappedPixels{};
            texture->GetDevice(d3dDevice.put());
            d3dDevice->GetImmediateContext(d3dDeviceContext.put());
            HRESULT mappingResult = d3dDeviceContext->Map(texture.get(), 0, D3D11_MAP_READ, 0, &mappedPixels); // Take texture access from GPU
            bool useStaging = mappingResult == E_INVALIDARG;

            winrt::com_ptr<ID3D11Texture2D> stagingTexture{};
            if ( useStaging )
            {
                D3D11_TEXTURE2D_DESC stagingDescriptor = textureDescriptor;
                stagingDescriptor.Usage = D3D11_USAGE::D3D11_USAGE_STAGING;
                stagingDescriptor.BindFlags = 0;
                stagingDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_READ;
                stagingDescriptor.MiscFlags = 0;

                if ( SUCCEEDED(d3dDevice->CreateTexture2D(&stagingDescriptor, NULL, stagingTexture.put())) )
                {
                    d3dDeviceContext->CopyResource(stagingTexture.get(), texture.get());
                    mappingResult = d3dDeviceContext->Map(stagingTexture.get(), 0, D3D11_MAP_READ, 0, &mappedPixels); // Take texture access from GPU
                }
            }

            if ( SUCCEEDED(mappingResult) )
            {
                std::uint8_t* pData = (std::uint8_t*)mappedPixels.pData;
                auto trimmedPixelData = std::make_unique<uint32_t[]>(size_t(4)*width*height);
                if ( size_t(mappedPixels.RowPitch) == size_t(4)*width ) // No row padding
                    std::memcpy(&trimmedPixelData[0], &pData[0], size_t(4)*width*height);
                else // pData contains row padding which brings row size up to "RowPitch", copy row by row
                {
                    for ( size_t y = 0; y < height; y++ )
                        std::memcpy(&trimmedPixelData[y*width], &pData[y*mappedPixels.RowPitch], size_t(4)*width);
                }

                d3dDeviceContext->Unmap(useStaging ? stagingTexture.get() : texture.get(), 0); // Give texture access back to GPU

                this->width = width;
                this->height = height;
                this->pixels.swap(trimmedPixelData);
                return true;
            }
            return false;
        }

        bool copyFromFrame(const winrt::Windows::Graphics::Capture::Direct3D11CaptureFrame & frame)
        {
            winrt::com_ptr<ID3D11Texture2D> texture;
            frame.Surface().as<Windows::Graphics::DirectX::Direct3D11::IDirect3DDxgiInterfaceAccess>()->GetInterface(
                winrt::guid_of<ID3D11Texture2D>(), texture.put_void());

            return copyFromTexture(texture);
        }

        PixelData() : width(0), height(0), pixels(nullptr) {}

        PixelData(winrt::com_ptr<ID3D11Texture2D> & texture) : width(0), height(0), pixels(nullptr)
        {
            copyFromTexture(texture);
        }

        PixelData(const winrt::Windows::Graphics::Capture::Direct3D11CaptureFrame & frame) : width(0), height(0), pixels(nullptr)
        {
            copyFromFrame(frame);
        }

        operator bool() const
        {
            return this->width > 0 && this->height > 0 && this->pixels != nullptr;
        }

        static bool captureFrameToFile(const winrt::Windows::Graphics::Capture::Direct3D11CaptureFrame & frame, const std::string & outFilePath) // example use
        {
            PixelData data(frame);
            if ( data )
            {
                std::ofstream out(outFilePath.c_str(), std::ios_base::out|std::ios_base::binary);
                out.write((const char*)&data.pixels[0], data.width*data.height*4);
                out.close();
                return true;
            }
            return false;
        }
    };

    CaptureSource::CaptureSource(FrameArrivedHandler frameArrivedHandler, SourceWindowClosedHandler sourceWindowClosedHandler,
        const winrt::Windows::Graphics::Capture::GraphicsCaptureItem & captureItem,
        winrt::Windows::Graphics::DirectX::DirectXPixelFormat pixelFormat,
        int32_t bufferCount) : captureItem(captureItem)
    {
        if ( !CaptureSource::d3dDevice )
        {
            winrt::com_ptr<ID3D11Device> d3d11Device;
            UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
            HRESULT result = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, flags, NULL, 0, D3D11_SDK_VERSION, d3d11Device.put(), NULL, NULL);
            if ( result == DXGI_ERROR_UNSUPPORTED )
                result = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_WARP, NULL, flags, NULL, 0, D3D11_SDK_VERSION, d3d11Device.put(), NULL, NULL);

            winrt::check_hresult(result);
            auto dxgiDevice = d3d11Device.as<IDXGIDevice>();

            winrt::com_ptr<IInspectable> graphicsDevice;
            winrt::check_hresult(CreateDirect3D11DeviceFromDXGIDevice(dxgiDevice.get(), graphicsDevice.put()));
            CaptureSource::d3dDevice = graphicsDevice.as<winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice>();
        }

        this->captureFramePool = winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool::Create(
            CaptureSource::d3dDevice, pixelFormat, bufferCount, captureItem.Size());
        this->captureSession = this->captureFramePool.CreateCaptureSession(captureItem);
        this->captureSession.IsCursorCaptureEnabled(false);
        this->captureFramePool.FrameArrived(frameArrivedHandler);
        this->captureSession.StartCapture();

        this->itemClosedRevoker.revoke();
        this->itemClosedRevoker = captureItem.Closed(winrt::auto_revoke, sourceWindowClosedHandler);
    }

    CaptureSource::~CaptureSource()
    {
        this->captureSession.Close();
        this->captureFramePool.Close();
    }

    void CaptureSource::recreateFramePool(const winrt::Windows::Graphics::SizeInt32 & size,
        const winrt::Windows::Graphics::DirectX::DirectXPixelFormat & pixelFormat,
        int32_t bufferCount)
    {
        this->captureFramePool.Recreate(CaptureSource::d3dDevice, pixelFormat, bufferCount, size);
    }

    auto CaptureSource::GetD3dDeviceInterface()
    {
        winrt::com_ptr<ID3D11Device> d3d11Device;
        CaptureSource::d3dDevice.as<Windows::Graphics::DirectX::Direct3D11::IDirect3DDxgiInterfaceAccess>()->GetInterface(
            winrt::guid_of<ID3D11Device>(), d3d11Device.put_void());
        return d3d11Device;
    }

    HWND Mirror::createMirror(HWND hParent, HWND sourceWindow, winrt::Windows::Graphics::DirectX::DirectXPixelFormat pixelFormat, UINT bufferCount)
    {
        this->clearClip();
        if ( this->graphicsCaptureSource != nullptr )
            reset();

        winrt::Windows::Graphics::Capture::GraphicsCaptureItem captureItem = { nullptr };
        auto captureFactory = winrt::get_activation_factory<winrt::Windows::Graphics::Capture::GraphicsCaptureItem, IGraphicsCaptureItemInterop>();
        try
        {
            winrt::check_hresult(captureFactory->CreateForWindow(
                sourceWindow, winrt::guid_of<ABI::Windows::Graphics::Capture::IGraphicsCaptureItem>(), winrt::put_abi(captureItem)));
        }
        catch ( const winrt::hresult_invalid_argument & invalidArg )
        {
            HWND rootSourceWindow = GetAncestor(sourceWindow, GA_ROOT);
            if ( rootSourceWindow != NULL && sourceWindow != rootSourceWindow ) // Try again from root window
            {
                winrt::check_hresult(captureFactory->CreateForWindow(
                    rootSourceWindow, winrt::guid_of<ABI::Windows::Graphics::Capture::IGraphicsCaptureItem>(), winrt::put_abi(captureItem)));

                sourceWindow = rootSourceWindow;
            }
            else
                throw invalidArg;
        }

        this->graphicsCaptureSource = std::make_unique<CaptureSource>(
            CaptureSource::FrameArrivedHandler{this, &Mirror::frameArrived},
            CaptureSource::SourceWindowClosedHandler{this, &Mirror::sourceClosed},
            captureItem, pixelFormat, bufferCount);

        this->size = captureItem.Size();
        this->pixelFormat = pixelFormat;
        this->bufferCount = bufferCount;

        createMirrorCasing(hParent);

        auto d3dDeviceInterface = CaptureSource::GetD3dDeviceInterface();
        d3dDeviceInterface->GetImmediateContext(this->d3dContext.put());

        DXGI_SWAP_CHAIN_DESC1 desc = {};
        desc.Width = static_cast<uint32_t>(this->size.Width);
        desc.Height = static_cast<uint32_t>(this->size.Height);
        desc.Format = static_cast<DXGI_FORMAT>(pixelFormat);
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.BufferCount = bufferCount;
        desc.Scaling = DXGI_SCALING_STRETCH;
        desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        desc.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;

        auto dxgiDevice = d3dDeviceInterface.as<IDXGIDevice2>();
        winrt::com_ptr<IDXGIAdapter> adapter;
        winrt::check_hresult(dxgiDevice->GetParent(winrt::guid_of<IDXGIAdapter>(), adapter.put_void()));
        winrt::com_ptr<IDXGIFactory2> factory;
        winrt::check_hresult(adapter->GetParent(winrt::guid_of<IDXGIFactory2>(), factory.put_void()));
        winrt::check_hresult(factory->CreateSwapChainForComposition(d3dDeviceInterface.get(), &desc, NULL, this->swapChain.put()));

        winrt::Windows::UI::Composition::ICompositionSurface surface {};
        auto compositorInterop = this->compositor.as<ABI::Windows::UI::Composition::ICompositorInterop>();
        winrt::com_ptr<ABI::Windows::UI::Composition::ICompositionSurface> compositionSurface;
        winrt::check_hresult(compositorInterop->CreateCompositionSurfaceForSwapChain(this->swapChain.get(), compositionSurface.put()));
        winrt::check_hresult(compositionSurface->QueryInterface(
            winrt::guid_of<winrt::Windows::UI::Composition::ICompositionSurface>(), winrt::put_abi(surface)));
        this->compositionSurfaceBrush.Surface(surface);
        return sourceWindow;
    }
    
    void Mirror::setClip(UINT left, UINT top, UINT right, UINT bottom)
    {
        this->clipRegion.left = left;
        this->clipRegion.top = top;
        this->clipRegion.front = 0;
        this->clipRegion.right = right;
        this->clipRegion.bottom = bottom;
        this->clipRegion.back = 1;
        this->clipped = true;
    }
    
    void Mirror::clearClip()
    {
        this->clipped = false;
        this->clipRegion.left = 0;
        this->clipRegion.top = 0;
        this->clipRegion.front = 0;
        this->clipRegion.right = 0;
        this->clipRegion.bottom = 0;
        this->clipRegion.back = 1;
    }

    void Mirror::reset()
    {
        this->clearClip();
        this->graphicsCaptureSource = nullptr;

        this->d3dContext = { nullptr };
        this->swapChain = { nullptr };
        this->size = {};
        this->pixelFormat = { winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized };
        this->bufferCount = 0;

        this->compositor = { nullptr };
        this->containerVisual = { nullptr };
        this->spriteVisual = { nullptr };
        this->compositionSurfaceBrush = { nullptr };
        this->desktopWindowTarget = { nullptr };
    }

    void Mirror::createMirrorCasing(HWND hParent)
    {
        this->compositor = winrt::Windows::UI::Composition::Compositor();

        this->containerVisual = this->compositor.CreateContainerVisual();
        this->containerVisual.RelativeSizeAdjustment({ 1.0f, 1.0f });

        this->spriteVisual = this->compositor.CreateSpriteVisual();
        this->spriteVisual.RelativeSizeAdjustment({ 1.0f, 1.0f });

        this->compositionSurfaceBrush = this->compositor.CreateSurfaceBrush();
        this->compositionSurfaceBrush.HorizontalAlignmentRatio(0.5f);
        this->compositionSurfaceBrush.VerticalAlignmentRatio(0.5f);
        this->compositionSurfaceBrush.Stretch(winrt::Windows::UI::Composition::CompositionStretch::Uniform);

        this->spriteVisual.Brush(this->compositionSurfaceBrush);
        this->containerVisual.Children().InsertAtTop(this->spriteVisual);

        auto compositorDesktopInterop = this->compositor.as<ABI::Windows::UI::Composition::Desktop::ICompositorDesktopInterop>();
        winrt::check_hresult(compositorDesktopInterop->CreateDesktopWindowTarget(hParent, true,
            reinterpret_cast<ABI::Windows::UI::Composition::Desktop::IDesktopWindowTarget**>(winrt::put_abi(this->desktopWindowTarget))));
        this->desktopWindowTarget.Root(this->containerVisual);
    }

    void Mirror::copyFrame(const winrt::Windows::Graphics::Capture::Direct3D11CaptureFrame & frame)
    {
        winrt::com_ptr<ID3D11Texture2D> sourceTexture;
        frame.Surface().as<Windows::Graphics::DirectX::Direct3D11::IDirect3DDxgiInterfaceAccess>()->GetInterface(
            winrt::guid_of<ID3D11Texture2D>(), sourceTexture.put_void());

        winrt::com_ptr<ID3D11Texture2D> backBuffer;
        winrt::check_hresult(this->swapChain->GetBuffer(0, winrt::guid_of<ID3D11Texture2D>(), backBuffer.put_void())); // Get back buffer from swapChain

        if ( this->clipped )
            this->d3dContext->CopySubresourceRegion(backBuffer.get(), 0, 0, 0, 0, sourceTexture.get(), 0, &clipRegion); // Copy clipped texture to back buffer
        else
            this->d3dContext->CopyResource(backBuffer.get(), sourceTexture.get()); // Copy surfaceTexture to the swap chain back buffer from source
    }

    void Mirror::renderFrame(const winrt::Windows::Graphics::Capture::Direct3D11CaptureFrame & frame)
    {
        copyFrame(frame);
        DXGI_PRESENT_PARAMETERS presentParameters{};
        this->swapChain->Present1(1, 0, &presentParameters); // Presents a frame to the display screen
    }

    void Mirror::frameArrived(const winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool & framePool, const winrt::Windows::Foundation::IInspectable &)
    {
        winrt::Windows::Graphics::Capture::Direct3D11CaptureFrame frame = framePool.TryGetNextFrame();
        bool swapChainChanged = updateSize(frame.ContentSize());
            
        renderFrame(frame);

        if ( swapChainChanged )
            this->graphicsCaptureSource->recreateFramePool(this->size, this->pixelFormat, this->bufferCount);
    }

    void Mirror::sourceClosed(const winrt::Windows::Graphics::Capture::GraphicsCaptureItem &, const winrt::Windows::Foundation::IInspectable &)
    {
        reset();
    }

    void Mirror::refreshSwapChain()
    {
        winrt::check_hresult(this->swapChain->ResizeBuffers(
            this->bufferCount, uint32_t(this->size.Width), uint32_t(this->size.Height), DXGI_FORMAT(this->pixelFormat), 0));
    }

    bool Mirror::updateSize(const winrt::Windows::Graphics::SizeInt32 & size)
    {
        if ( size.Width != this->size.Width || size.Height != this->size.Height )
        {
            this->size.Width = size.Width;
            this->size.Height = size.Height;
            refreshSwapChain();
            return true;
        }
        return false;
    }

    bool Mirror::updatePixelFormat(const winrt::Windows::Graphics::DirectX::DirectXPixelFormat & pixelFormat)
    {
        if ( pixelFormat != this->pixelFormat )
        {
            this->pixelFormat = pixelFormat;
            refreshSwapChain();
            return true;
        }
        return false;
    }
}
