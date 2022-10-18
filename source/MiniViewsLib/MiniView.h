#ifndef MINIVIEW_H
#define MINIVIEW_H
#include "../WindowsLib/WindowsUi.h"
#include "WinrtGraphicsCapture.h"

class MiniView;

class WinImage
{
public:
    virtual ~WinImage();

    LONG getWidth() const;
    LONG getHeight() const;

    virtual bool isValid() = 0;

protected:
    void setDimensions(LONG width, LONG height);

private:
    LONG width = 0;
    LONG height = 0;
};

class WinGdiImage : public WinImage
{
public:
    WinGdiImage(WinLib::WindowsItem & windowsItem);
    WinGdiImage(HWND hSource);
    virtual ~WinGdiImage();

    bool isValid() override;
    void stretchBlt(HDC hdcDest, int xDest, int yDest, int wDest, int hDest,
        int xSrc, int ySrc, int wSrc, int hSrc, DWORD rop);

private:
    void SetImage(HDC sourceDc, LONG width, LONG height);
    void SetBmi();

    HDC hMemDc;
    BITMAPINFO bitmapInfo;
    HBITMAP bitmap;

    WinGdiImage() = delete;
};

class WindowsCaptureImage;

class IMiniViewUser
{
    public:
        virtual void NotifyChange(MiniView &miniView) = 0; // Sent when the user causes a change to the MiniView
        virtual void CloseMiniView(MiniView &miniView) = 0; // Sent when a MiniView believes it should be destroyed
        virtual void SetClipRegion(MiniView &miniView) = 0; // Sent when a MiniView wants to adjust its clip area
        virtual bool GetDefaultMatchSourcePosition(MiniView &miniView) = 0;
        virtual bool GetDefaultMatchSourceSize(MiniView &miniView) = 0;
        virtual bool GetDefaultLockSizeRatio(MiniView &miniView) = 0;
        virtual bool GetDefaultHideWhenSourceOnTop(MiniView &miniView) = 0;
        virtual bool GetUseCachedImageWhenFrozen(MiniView &miniView) = 0;
        virtual bool GetShowFrozenIndicatorIcon(MiniView &miniView) = 0;
        virtual bool GetShowFrozenContextMenuItem(MiniView &miniView) = 0;
};

class MiniView : public WinLib::ClassWindow
{
    public:

        MiniView(IMiniViewUser* user);
        ~MiniView();
        bool CreateThis(HWND hParent, HWND hSource, int xc, int yc);
        void DestroyThis();
        static void ClearStaticCache();
        HWND GetSourceWindow();

        bool IsClipped();
        RECT GetClipRect();
        void SetClip(bool clipped, int left, int top, int right, int bottom);
        void SetClipRegion();
        void ClearClipRegion();
        void AdjustClipTop(int newTop);
        void AdjustClipLeft(int newLeft);
        void AdjustClipRight(int newRight);
        void AdjustClipBottom(int newBottom);

        void AdjustWindowLeft(int newLeft);
        void AdjustWindowTop(int newTop);
        void AdjustCliWidth(int newCliWidth);
        void AdjustCliHeight(int newCliHeight);

        void FrozenInfo();
        void SetEditMode(bool editMode);
        void ValidateProperties();
        void ValidateImage();
        void RunHalfSecondActions();
        void RunFrame();
        void ShowByParentRequest();
        void HideByParentRequest();
        void ToggleHideWhenSourceOnTop();
        void ToggleLockRatio();
        bool SizeRatioLocked();
        bool HidesWhenSourceOnTop();
        void MatchSource(bool matchPos, bool matchSize);


    protected:

        void PaintInstructions();
        void PaintMiniView();
        bool GetClientScreenTopLeft(int &left, int &top);
        bool ClientRectToWindowSize(RECT cliRect, int &width, int &height);
        bool GetTopLeftBorderSize(int &leftBorderWidth, int &topBorderHeight);
        bool GetBorderSize(int &width, int &height);
        void SetSourceHandle(HWND hWnd);
        bool IsPerfectSize();
        void FixRatio(); // Adjusts the internal paint area to match the source size ratio
        void ClearFixRatioCache();
        void DoSizing(WPARAM wParam, RECT* rect);
        void SizeFinished();
        void WindowMoved();
        void SetMinMaxSize(MINMAXINFO &minMaxInfo);
        void ExitSizeMove();
        bool IsSourceOnTop();
        void CheckHideBySourceOnTop();
        void RunContextMenu();
        void PostCloseMessage();
        void ProcessCloseMessage();
        void HideCommand();
        void OpenProperties();
        void BlackoutMiniView();
        int EraseBackground();
        LRESULT NonClientHitTest(WPARAM wParam, LPARAM lParam);
        LRESULT MouseMove(WPARAM wParam, LPARAM lParam);
        LRESULT SetCursor(WPARAM wParam, LPARAM lParam);
        LRESULT LButtonDown(WPARAM wParam, LPARAM lParam);
        LRESULT LButtonUp(WPARAM wParam, LPARAM lParam);
        LRESULT RButtonDown(WPARAM wParam, LPARAM lParam);
        LRESULT RButtonUp(WPARAM wParam, LPARAM lParam);
        LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


    private:

        WinrtGraphics::Mirror graphicsCaptureMirror; // Graphics capture mirror, only used if BitBlt fails
        HWND hSource; // A handle to the window the MiniView is viewing
        bool settingWindow; // Whether this MiniView is currently being set
        bool editMode; // Whether this MiniView is currently in edit mode
        bool hideWhenSourceOnTop;
        bool hiddenBySourceOnTop;
        bool hiddenByParent;
        bool isGdiCompatible;
        bool isGraphicsCaptureCompatible;
        bool isFrozen;

        bool lockRatio; // Determines whether the size ratio of the MiniView to the source should be maintained
        int lastUserSetCliWidth, lastUserSetCliHeight,
        //    lastFixedCliWidth, lastFixedCliHeight,
            lastCliWidthFixedTo, lastCliHeightFixedTo;
        bool clipped; // Whether this mini-window is clipping parts of the source window
        RECT rcClip; // The dimensions the MiniView is viewing/clipping dimensions
        bool internallyClipped; // Whether this mini-window is only drawing to a portion of itself
        RECT rcInternalClip; // The internal area of a mini-view to which the source window is being copied
        HBRUSH whiteBrush;
        HBRUSH blackBrush;
        IMiniViewUser* user;
        std::unique_ptr<WinGdiImage> winGdiImageCache;
        SIZE sourceSize;

        static const int minimumDimension; // The minimum width and height of the MiniView
        static const DWORD constantStyles; // Styles always applied to the window
        static const DWORD editModeStyles; // The styles that are applied while in edit mode
        static HICON frozenIcon;
};

class WindowsCaptureImage : public WinImage
{
public:
    virtual ~WindowsCaptureImage();

    bool isValid() override;

private:
    WindowsCaptureImage() = delete;
};

void DrawWrappableString(HDC hDC, const std::string & str, int startX, int startY, int cliWidth, int cliHeight);

#endif