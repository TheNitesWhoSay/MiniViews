#include "MiniView.h"
#include "Resource.h"
#include "Preferences.h"
#include <cstdlib>
#include <optional>
#include "Zerocmp.h"
#include "Math.h"

enum class Id : int
{
    ClearClipRegion = ID_FIRST,
    SetClipRegion,
    Frozen,
    LockRatio,
    MatchSource,
    HideWhenSourceOnTop,
    Hide,
    Close,
    Properties
};

const int MiniView::minimumDimension(33); // Obligatory definition of static variable

const DWORD MiniView::constantStyles(WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS);

const DWORD MiniView::editModeStyles(WS_THICKFRAME|WS_CAPTION|WS_SYSMENU);

HICON MiniView::frozenIcon(NULL);

MiniView::MiniView(IMiniViewUser* user) : graphicsCaptureMirror(), hSource(NULL), settingWindow(true), editMode(true),
    hideWhenSourceOnTop(user->GetDefaultHideWhenSourceOnTop(*this)),
    hiddenBySourceOnTop(true), hiddenByParent(false),
    isGdiCompatible(false), isGraphicsCaptureCompatible(false), isFrozen(false),
    lockRatio(user->GetDefaultLockSizeRatio(*this)),
    lastUserSetCliWidth(0), lastUserSetCliHeight(0),
    //lastFixedCliWidth(0), lastFixedCliHeight(0),
    lastCliWidthFixedTo(0), lastCliHeightFixedTo(0),
    clipped(false), internallyClipped(false), user(user)
{
    rcClip.left = 0;
    rcClip.top = 0;
    rcClip.right = 0;
    rcClip.bottom = 0;
    rcInternalClip.left = 0;
    rcInternalClip.top = 0;
    rcInternalClip.right = 0;
    rcInternalClip.bottom = 0;
}

MiniView::~MiniView()
{

}

bool MiniView::CreateThis(HWND hParent, HWND sourceHandle, int xc, int yc)
{
    if ( MiniView::frozenIcon == NULL )
        MiniView::frozenIcon = WinLib::ResourceManager::getIcon(IDI_FROZEN, 0, 0, LR_LOADTRANSPARENT);

    hSource = sourceHandle;
    auto hIcon = WinLib::ResourceManager::getIcon(IDI_MINIVIEWSICON, 32, 32);
    auto hIconSmall = WinLib::ResourceManager::getIcon(IDI_MINIVIEWSICON, 16, 16);
    if ( ClassWindow::RegisterWindowClass(NULL, hIcon, NULL, NULL, NULL, "MiniViewWin", hIconSmall, false) )
    {
        ::GetClientRect(hSource, &rcClip);
        int initWidth = (rcClip.right - rcClip.left) / 6;
        int initHeight = (rcClip.bottom - rcClip.top) / 6;
        if ( ClassWindow::CreateClassWindow( NULL, "Mini View", constantStyles,
                                             xc-initWidth/2, yc-initHeight/2, initWidth, initHeight, hParent, NULL ) )
        {
            SetEditMode(true);
            WindowsItem::SetWinLong(GWL_EXSTYLE, WindowsItem::GetWinLong(GWL_EXSTYLE&(~WS_EX_TRANSPARENT)));
            ::SetWindowPos(getHandle(), NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
            PaintInstructions();
            return true;
        }
    }
    return false;
}

void MiniView::DestroyThis()
{
    ClassWindow::DestroyThis();
}

void MiniView::ClearStaticCache()
{
    if ( MiniView::frozenIcon != NULL )
        DestroyIcon(MiniView::frozenIcon);
}

HWND MiniView::GetSourceWindow()
{
    return hSource;
}

bool MiniView::IsClipped()
{
    return clipped;
}

RECT MiniView::GetClipRect()
{
    return rcClip;
}

void MiniView::SetClip(bool isClipped, int left, int top, int right, int bottom)
{
    BlackoutMiniView();
    clipped = isClipped;
    if ( clipped )
    {
        rcClip.left = left;
        rcClip.top = top;
        rcClip.right = right;
        rcClip.bottom = bottom;

        if ( user != nullptr )
        {
            bool matchSourcePos = user->GetDefaultMatchSourcePosition(*this);
            bool matchSourceSize = user->GetDefaultMatchSourceSize(*this);
            if ( matchSourcePos || matchSourceSize )
                MatchSource(matchSourcePos, matchSourceSize);
        }

        FixRatio();
        
        RECT sourceClientRect = {};
        ::GetClientRect(hSource, &sourceClientRect);
        LONG wSrc = sourceClientRect.right - sourceClientRect.left,
             hSrc = sourceClientRect.bottom - sourceClientRect.top;
        if ( this->isGraphicsCaptureCompatible )
            this->graphicsCaptureMirror.setClip(rcClip.left, rcClip.top, rcClip.right, rcClip.bottom, wSrc, hSrc);
    }
}

void MiniView::SetClipRegion()
{
    BlackoutMiniView();
    int borderWidth = 0, borderHeight = 0;
    GetBorderSize(borderWidth, borderHeight);
    WindowsItem::SetWidth(lastUserSetCliWidth+borderWidth);
    WindowsItem::SetHeight(lastUserSetCliHeight+borderHeight);
    if ( user != nullptr )
        user->SetClipRegion(*this);
}

void MiniView::ClearClipRegion()
{
    BlackoutMiniView();
    if ( this->isGraphicsCaptureCompatible )
        this->graphicsCaptureMirror.clearClip();
    int borderWidth = 0, borderHeight = 0;
    GetBorderSize(borderWidth, borderHeight);
    WindowsItem::SetWidth(lastUserSetCliWidth + borderWidth);
    WindowsItem::SetHeight(lastUserSetCliHeight + borderHeight);
    clipped = false;
    ::GetClientRect(hSource, &rcClip);
    FixRatio();
}

void MiniView::AdjustClipTop(int newTop)
{
    ClearFixRatioCache();
    RECT rcSource = {};
    if ( ::GetClientRect(hSource, &rcSource) == TRUE )
    {
        if ( newTop < rcSource.top )
            rcClip.top = rcSource.top;
        else if ( newTop > rcSource.bottom )
            rcClip.top = rcSource.bottom;
        else
            rcClip.top = newTop;
    }
}

void MiniView::AdjustClipLeft(int newLeft)
{
    ClearFixRatioCache();
    RECT rcSource = {};
    if ( ::GetClientRect(hSource, &rcSource) == TRUE )
    {
        if ( newLeft < rcSource.left )
            rcClip.left = rcSource.left;
        else if ( newLeft > rcSource.right )
            rcClip.left = rcSource.right;
        else
            rcClip.left = newLeft;
    }
}

void MiniView::AdjustClipRight(int newRight)
{
    ClearFixRatioCache();
    RECT rcSource = {};
    if ( ::GetClientRect(hSource, &rcSource) == TRUE )
    {
        if ( newRight < rcSource.left )
            rcClip.right = rcSource.left;
        else if ( newRight > rcSource.right )
            rcClip.right = rcSource.right;
        else
            rcClip.right = newRight;
    }
}

void MiniView::AdjustClipBottom(int newBottom)
{
    ClearFixRatioCache();
    RECT rcSource = {};
    if ( ::GetClientRect(hSource, &rcSource) == TRUE )
    {
        if ( newBottom < rcSource.top )
            rcClip.bottom = rcSource.top;
        else if ( newBottom > rcSource.bottom )
            rcClip.bottom = rcSource.bottom;
        else
            rcClip.bottom = newBottom;
    }
}

void MiniView::AdjustWindowLeft(int newLeft)
{
    WindowsItem::MoveTo(newLeft, Top());
}

void MiniView::AdjustWindowTop(int newTop)
{
    WindowsItem::MoveTo(Left(), newTop);
}

void MiniView::AdjustCliWidth(int newCliWidth)
{
    int borderWidth = 0, borderHeight = 0;
    GetBorderSize(borderWidth, borderHeight);
    int newCliHeight = cliHeight();
    if ( lockRatio )
    {
        RECT sourceRect = {};
        ::GetClientRect(hSource, &sourceRect);
        lastCliWidthFixedTo = clipped ? rcClip.right - rcClip.left : sourceRect.right - sourceRect.left;
        lastCliHeightFixedTo = clipped ? rcClip.bottom - rcClip.top : sourceRect.bottom - sourceRect.top;

        newCliHeight = RoundedQuotient(newCliWidth*lastCliHeightFixedTo, lastCliWidthFixedTo);
    }

    lastUserSetCliWidth = newCliWidth;
    lastUserSetCliHeight = newCliHeight;
    //lastFixedCliWidth = newCliWidth;
    //lastFixedCliHeight = newCliHeight;
    WindowsItem::SetSize(newCliWidth+borderWidth, newCliHeight+borderHeight);
}

void MiniView::AdjustCliHeight(int newCliHeight)
{
    int borderWidth = 0, borderHeight = 0;
    GetBorderSize(borderWidth, borderHeight);
    int newCliWidth = cliWidth();
    if ( lockRatio )
    {
        RECT sourceRect = {};
        ::GetClientRect(hSource, &sourceRect);
        lastCliWidthFixedTo = clipped ? rcClip.right - rcClip.left : sourceRect.right - sourceRect.left;
        lastCliHeightFixedTo = clipped ? rcClip.bottom - rcClip.top : sourceRect.bottom - sourceRect.top;
        newCliWidth = RoundedQuotient(newCliHeight*lastCliWidthFixedTo, lastCliHeightFixedTo);
    }

    lastUserSetCliWidth = newCliWidth;
    lastUserSetCliHeight = newCliHeight;
    //lastFixedCliWidth = newCliWidth;
    //lastFixedCliHeight = newCliHeight;
    WindowsItem::SetSize(newCliWidth+borderWidth, newCliHeight+borderHeight);
}

void MiniView::FrozenInfo()
{
    WinLib::Message(std::string("Your source window has stopped rendering itself!\n\n") +
        "MiniViews is showing a cached image and the frozen indicator.\n\n" +
        "You may be able to prevent your source window from freezing via a setting in your source application," +
        "e.g. if your source window is chrome, disable chrome://flags/#calculate-native-win-occlusion\n\n" +
        "You can adjust MiniViews' behavior for frozen sources in the Advanced tab.");
}

void MiniView::SetEditMode(bool newStatus)
{
    editMode = newStatus;

    int newWidth = 0, newHeight = 0, oldLeft = 0, oldTop = 0, newLeft = 0, newTop = 0;
    RECT cliRect = {};
    WindowsItem::getClientRect(cliRect);
    GetClientScreenTopLeft(oldLeft, oldTop);

    if ( editMode ) // Going into edit mode
    {
        WindowsItem::SetWinLong(GWL_STYLE, WindowsItem::GetWinLong(GWL_STYLE) | editModeStyles);
        Show();
    }
    else // Leaving edit mode
        WindowsItem::SetWinLong(GWL_STYLE, WindowsItem::GetWinLong(GWL_STYLE) & (~editModeStyles));

    WindowsItem::RefreshFrame();

    GetClientScreenTopLeft(newLeft, newTop);
    ClientRectToWindowSize(cliRect, newWidth, newHeight);
    WindowsItem::SetPos(Left() + (oldLeft - newLeft), Top() + (oldTop - newTop), newWidth, newHeight);
}

void MiniView::ValidateProperties()
{
    if ( ::IsWindow(hSource) == 0 )
        PostCloseMessage();
    else
    {
        CheckHideBySourceOnTop();
        
        RECT rcSource;
        ::GetClientRect(this->hSource, &rcSource);
        LONG sourceWidth = rcSource.right-rcSource.left;
        LONG sourceHeight = rcSource.bottom-rcSource.top;
        if ( this->sourceSize.cx != sourceWidth || this->sourceSize.cy != sourceHeight ) // Source window size changed
        {
            this->sourceSize.cx = sourceWidth;
            this->sourceSize.cy = sourceHeight;

            FixRatio();
            if ( this->isGraphicsCaptureCompatible )
                this->graphicsCaptureMirror.handleSourceSizeChange(sourceWidth, sourceHeight);
        }
    }
}

void MiniView::ValidateImage()
{
    if ( isFrozen ) // Check for unfreeze opportunity...
    {
        if ( this->isGdiCompatible )
        {
            WinGdiImage winGdiImage(hSource);
            if ( winGdiImage.isValid() )
                this->isFrozen = false;
        }
        else if ( this->isGraphicsCaptureCompatible )
        {
            // TODO: Check for graphics capture unfreeze opportunity
        }
    }
    else if ( this->isGdiCompatible ) // Cache current image if valid
    {
        auto winGdiImage = std::make_unique<WinGdiImage>(hSource);
        if ( winGdiImage->isValid() )
            this->winGdiImageCache.swap(winGdiImage);
        else
            this->isFrozen = true;
    }
    else if ( this->isGraphicsCaptureCompatible ) // Cache current image if valid
    {
        // TODO: Implement me
    }
}

void MiniView::RunHalfSecondActions()
{
    ValidateProperties();
    if ( !settingWindow )
        ValidateImage();
}

void MiniView::RunFrame()
{
    if ( !settingWindow )
        PaintMiniView();
}

void MiniView::ShowByParentRequest()
{
    if ( hiddenByParent && !hiddenBySourceOnTop )
        WindowsItem::Show();

    hiddenByParent = false;
}

void MiniView::HideByParentRequest()
{
    if ( !hiddenByParent )
        WindowsItem::Hide();

    hiddenByParent = true;
}

void MiniView::ToggleHideWhenSourceOnTop()
{
    if ( hideWhenSourceOnTop && hiddenBySourceOnTop )
    {
        Show();
        hiddenBySourceOnTop = false;
    }

    hideWhenSourceOnTop = !hideWhenSourceOnTop;
    CheckHideBySourceOnTop();
}

void MiniView::ToggleLockRatio()
{
    lockRatio = !lockRatio;
    FixRatio();
}

bool MiniView::SizeRatioLocked()
{
    return lockRatio;
}

bool MiniView::HidesWhenSourceOnTop()
{
    return hideWhenSourceOnTop;
}

void MiniView::MatchSource(bool matchPos, bool matchSize)
{
    RECT sourceClientRect = {};
    ::GetClientRect(hSource, &sourceClientRect);
    POINT sourceTopLeft = {};
    sourceTopLeft.x = sourceClientRect.left;
    sourceTopLeft.y = sourceClientRect.top;
    ::ClientToScreen(hSource, &sourceTopLeft);

    int borderWidth = 0, borderHeight = 0, topLeftBorderWidth = 0, topLeftBorderHeight = 0;
    if ( GetBorderSize(borderWidth, borderHeight) && GetTopLeftBorderSize(topLeftBorderWidth, topLeftBorderHeight) )
    {
        int left = 0, top = 0, width = 0, height = 0;
        if ( clipped )
        {
            left = sourceTopLeft.x - topLeftBorderWidth + rcClip.left;
            top = sourceTopLeft.y - topLeftBorderHeight + rcClip.top;
            width = rcClip.right - rcClip.left + borderWidth;
            height = rcClip.bottom - rcClip.top + borderHeight;
        }
        else
        {
            left = sourceTopLeft.x - topLeftBorderWidth;
            top = sourceTopLeft.y - topLeftBorderHeight;
            width = sourceClientRect.right - sourceClientRect.left + borderWidth;
            height = sourceClientRect.bottom - sourceClientRect.top + borderHeight;
        }

        if ( matchPos && matchSize )
            WindowsItem::SetPos(left, top, width, height);
        else if ( matchPos )
            WindowsItem::MoveTo(left, top);
        else if ( matchSize )
            WindowsItem::SetSize(width, height);

        if ( matchSize )
        {
            internallyClipped = false;
            lastUserSetCliWidth = cliWidth();
            lastUserSetCliHeight = cliHeight();
        }

        if ( this->clipped && this->isGraphicsCaptureCompatible )
        {
            LONG wSrc = sourceClientRect.right - sourceClientRect.left,
                 hSrc = sourceClientRect.bottom - sourceClientRect.top;
            this->graphicsCaptureMirror.createMirror(WindowsItem::getHandle(), hSource);
            this->graphicsCaptureMirror.setClip(rcClip.left, rcClip.top, rcClip.right, rcClip.bottom, wSrc, hSrc);
        }
    }
}

void MiniView::PaintInstructions()
{
    RECT rcCli {};
    WindowsItem::getClientRect(rcCli);
    WinLib::DeviceContext dc { getHandle() };
    dc.setBuffer(rcCli);
    dc.fillRect(rcCli, RGB(255, 255, 255));
    dc.drawWrappableText("Drop me onto a window!", rcCli);
    dc.flushBuffer();
}

RECT getDpiScaledClip(const RECT & rcClip, LONG sourceDpi, LONG destDpi)
{
    RECT rect = {};
    rect.left = rcClip.left*sourceDpi/destDpi;
    rect.top = rcClip.top*sourceDpi/destDpi;
    rect.right = rcClip.right*sourceDpi/destDpi;
    rect.bottom = rcClip.bottom*sourceDpi/destDpi;
    return rect;
}

void MiniView::PaintMiniView()
{
    // If not GDI compatible, this method does nothing and graphicsCaptureMirror is responsible for graphic replication
    if ( this->isGdiCompatible ) 
    {
        RECT rcCli {};
        WindowsItem::getClientRect(rcCli);
        auto paintWidth = rcCli.right-rcCli.left;
        auto paintHeight = rcCli.bottom-rcCli.top;

        WinLib::DeviceContext sourceDc { hSource };
        WinLib::DeviceContext miniViewDc { getHandle() };
        if ( internallyClipped || isFrozen )
            miniViewDc.setBuffer(rcCli);

        miniViewDc.setBkMode(0);
        miniViewDc.setStretchBltMode(COLORONCOLOR);

        int xDest = internallyClipped ? rcInternalClip.left : 0,
            yDest = internallyClipped ? rcInternalClip.top : 0,
            wDest = internallyClipped ? rcInternalClip.right - rcInternalClip.left : paintWidth,
            hDest = internallyClipped ? rcInternalClip.bottom - rcInternalClip.top : paintHeight;

        HWND hMiniView = WindowsItem::getHandle();
        LONG sourceDpi = LONG(GetDpiForWindow(hSource)),
             destDpi = LONG(GetDpiForWindow(hMiniView));

        if ( clipped )
        {
            RECT rcDpiClip = getDpiScaledClip(this->rcClip, sourceDpi, destDpi);
            int clipWidth = (rcDpiClip.right - rcDpiClip.left),
                clipHeight = (rcDpiClip.bottom - rcDpiClip.top);

            if ( internallyClipped )
                miniViewDc.fillSysRect(rcCli, COLOR_BACKGROUND);

            if ( isFrozen && (user == nullptr || user->GetUseCachedImageWhenFrozen(*this)) && this->winGdiImageCache != nullptr )
                this->winGdiImageCache->stretchBlt(miniViewDc, xDest, yDest, wDest, hDest, rcDpiClip.left, rcDpiClip.top, clipWidth, clipHeight, SRCCOPY);
            else
                sourceDc.stretchBlt(miniViewDc, xDest, yDest, wDest, hDest, rcDpiClip.left, rcDpiClip.top, clipWidth, clipHeight, SRCCOPY);
        }
        else // Not clipped
        {
            RECT rcSource = {};
            ::GetClientRect(hSource, &rcSource);
            int sourceWidth = int((rcSource.right - rcSource.left)*sourceDpi/destDpi),
                sourceHeight = int((rcSource.bottom - rcSource.top)*sourceDpi/destDpi);

            RECT rcDest = {};
            ::GetClientRect(hMiniView, &rcDest);
            if ( internallyClipped )
                miniViewDc.fillSysRect(rcCli, COLOR_BACKGROUND);

            if (isFrozen && (user == nullptr || user->GetUseCachedImageWhenFrozen(*this)) && this->winGdiImageCache != nullptr)
                this->winGdiImageCache->stretchBlt(miniViewDc, xDest, yDest, wDest, hDest, 0, 0, sourceWidth, sourceHeight, SRCCOPY);
            else
                sourceDc.stretchBlt(miniViewDc, xDest, yDest, wDest, hDest, 0, 0, sourceWidth, sourceHeight, SRCCOPY);
        }

        if ( isFrozen && (user == nullptr || user->GetShowFrozenIndicatorIcon(*this)) && MiniView::frozenIcon != NULL )
            miniViewDc.drawIconEx(wDest-32, 0, MiniView::frozenIcon, 0, 0, 0, NULL, DI_NORMAL);

        miniViewDc.flushBuffer();
    }
}

bool MiniView::GetClientScreenTopLeft(int &left, int &top)
{
    RECT cliRect = {};
    if ( WindowsItem::getClientRect(cliRect) )
    {
        POINT cliTopLeft = {};
        cliTopLeft.x = cliRect.left;
        cliTopLeft.y = cliRect.top;
        if ( ::ClientToScreen(getHandle(), &cliTopLeft) != 0 )
        {
            left = cliTopLeft.x;
            top = cliTopLeft.y;
            return true;
        }
    }
    return false;
}

bool MiniView::ClientRectToWindowSize(RECT cliRect, int &width, int &height)
{
    if ( ::AdjustWindowRect(&cliRect, WindowsItem::GetWinLong(GWL_STYLE), FALSE) == TRUE )
    {
        width = cliRect.right - cliRect.left;
        height = cliRect.bottom - cliRect.top;
        return true;
    }
    return false;
}

bool MiniView::GetBorderSize(int &width, int &height)
{
    RECT winRect = {}, cliRect = {};
    if ( getWindowRect(winRect) && getClientRect(cliRect) )
    {
        width = (winRect.right - winRect.left) - (cliRect.right - cliRect.left);
        height = (winRect.bottom - winRect.top) - (cliRect.bottom - cliRect.top);
        return true;
    }
    return false;
}

bool MiniView::GetTopLeftBorderSize(int &leftBorderWidth, int &topBorderHeight)
{
    RECT winRect = {}, cliRect = {};
    if ( getWindowRect(winRect) && getClientRect(cliRect) )
    {
        POINT winTopLeft = {}, cliTopLeft = {};
        winTopLeft.x = winRect.left;
        winTopLeft.y = winRect.top;
        cliTopLeft.x = cliRect.left;
        cliTopLeft.y = cliRect.top;
        if ( ClientToScreen(getParent(), &winTopLeft) != 0 && ClientToScreen(getHandle(), &cliTopLeft) != 0 )
        {
            leftBorderWidth = cliTopLeft.x - winTopLeft.x;
            topBorderHeight = cliTopLeft.y - winTopLeft.y;
            return true;
        }
    }
    return false;
}

void MiniView::SetSourceHandle(HWND newSourceHandle)
{
    hSource = newSourceHandle;
}

bool MiniView::IsPerfectSize()
{
    if ( clipped )
    {
        return cliWidth() == rcClip.right - rcClip.left &&
            cliHeight() == rcClip.bottom - rcClip.top;
    }
    else
    {
        RECT sourceRect = {};
        ::GetClientRect(hSource, &sourceRect);
        return cliWidth() == sourceRect.right - sourceRect.left &&
            cliHeight() == sourceRect.bottom - sourceRect.top;
    }
}

void MiniView::FixRatio()
{
    RECT cliRect = {};
    WindowsItem::getClientRect(cliRect);

    if ( lockRatio && !settingWindow )
    {
        RECT sourceRect = {};
        ::GetClientRect(hSource, &sourceRect);

        int borderWidth = 0, borderHeight = 0;
        GetBorderSize(borderWidth, borderHeight);

        int guideMiniWidth = lastUserSetCliWidth;
        int guideMiniHeight = lastUserSetCliHeight;
        int sourceWidth = clipped ? rcClip.right - rcClip.left : sourceRect.right - sourceRect.left;
        int sourceHeight = clipped ? rcClip.bottom - rcClip.top : sourceRect.bottom - sourceRect.top;

        if ( sourceWidth != lastCliWidthFixedTo || sourceHeight != lastCliHeightFixedTo )
        {
            int cw = guideMiniWidth, ch = guideMiniHeight;
            if ( sourceWidth <= guideMiniWidth && sourceHeight <= guideMiniHeight ) // Both source dimensions smaller than MiniView
            {
                if ( sourceWidth*guideMiniHeight > sourceHeight*guideMiniWidth && sourceWidth > 0 ) // sourceWidth/miniWidth > sourceHeight/miniHeight
                    ch = RoundedQuotient(cw*sourceHeight, sourceWidth);
                else if ( sourceHeight*guideMiniWidth > sourceWidth*guideMiniHeight && sourceHeight > 0 ) // sourceHeight/miniHeight > sourceWidth/miniWidth
                    cw = RoundedQuotient(ch*sourceWidth, sourceHeight);
            }
            else if ( sourceWidth > guideMiniWidth && sourceHeight <= guideMiniHeight && sourceWidth > 0 ) // Only sourceWidth larger than MiniView
                ch = RoundedQuotient(cw*sourceHeight, sourceWidth);
            else if ( sourceWidth <= guideMiniWidth && sourceHeight > guideMiniHeight && sourceHeight > 0 ) // Only sourceHeight larger than MiniView
                cw = RoundedQuotient(ch*sourceWidth, sourceHeight);
            else if ( sourceWidth > guideMiniWidth && sourceHeight > guideMiniHeight ) // Both source dimensions larger than MiniView
            {
                if ( guideMiniWidth*sourceHeight > guideMiniHeight*sourceWidth && sourceHeight > 0 ) // miniWidth/sourceWidth > miniHeight/sourceHeight
                    cw = RoundedQuotient(ch*sourceWidth, sourceHeight);
                else if ( guideMiniHeight*sourceWidth > guideMiniWidth*sourceHeight && sourceWidth > 0 ) // miniHeight/sourceHeight > miniWidth/sourceWidth
                    ch = RoundedQuotient(cw*sourceHeight, sourceWidth);
            }

            if ( cw != guideMiniWidth || ch != guideMiniHeight )
            {
                internallyClipped = true;
                rcInternalClip.left = RoundedQuotient(std::abs(guideMiniWidth - cw), 2);
                rcInternalClip.top = RoundedQuotient(std::abs(guideMiniHeight - ch), 2);
                rcInternalClip.right = rcInternalClip.left + cw;
                rcInternalClip.bottom = rcInternalClip.top + ch;
            }
            else
                internallyClipped = false;

            lastCliWidthFixedTo = sourceWidth;
            lastCliHeightFixedTo = sourceHeight;
        }
    }
}

void MiniView::ClearFixRatioCache()
{
    //lastFixedCliWidth = 0;
    //lastFixedCliHeight = 0;
    internallyClipped = false;
    lastCliWidthFixedTo = 0;
    lastCliHeightFixedTo = 0;
}

void MiniView::DoSizing(WPARAM wParam, RECT* rect)
{
    ClearFixRatioCache();
    int borderWidth = 0, borderHeight = 0;
    GetBorderSize(borderWidth, borderHeight);

    if ( settingWindow )
        PaintInstructions();
    else if ( lockRatio )
    {
        RECT sourceRect = {};
        ::GetClientRect(hSource, &sourceRect);

        int miniWidth = rect->right - rect->left - borderWidth;
        int miniHeight = rect->bottom - rect->top - borderHeight;
        int sourceWidth = clipped ? rcClip.right - rcClip.left : sourceRect.right - sourceRect.left;
        int sourceHeight = clipped ? rcClip.bottom - rcClip.top : sourceRect.bottom - sourceRect.top;

        if ( wParam == WMSZ_BOTTOM || wParam == WMSZ_TOP || // Grabbed bottom or top or a corner and the width is off
            (miniWidth*sourceHeight > sourceWidth*miniHeight && wParam != WMSZ_LEFT && wParam != WMSZ_RIGHT) ) // Match width to height
        {
            if ( wParam == WMSZ_BOTTOMLEFT || wParam == WMSZ_TOPLEFT )
                rect->left = rect->right - RoundedQuotient(miniHeight*sourceWidth, sourceHeight) - borderWidth;
            else // WMSZ_BOTTOMRIGHT || wParam == WMSZ_TOPRIGHT || wParam == WMSZ_TOP || wParam == WMSZ_BOTTOM
                rect->right = rect->left + RoundedQuotient(miniHeight*sourceWidth, sourceHeight) + borderWidth;
        }
        else //if ( miniWidth*sourceHeight < sourceWidth*miniHeight ) // Not bottom or top and height is off: Match height to width
        {
            if ( wParam == WMSZ_TOPRIGHT || wParam == WMSZ_TOPLEFT )
                rect->top = rect->bottom - RoundedQuotient(miniWidth*sourceHeight, sourceWidth) - borderHeight;
            else // WMSZ_BOTTOMRIGHT || wParam == WMSZ_BOTTOMLEFT || wParam == WMSZ_RIGHT || wParam == WMSZ_LEFT
                rect->bottom = rect->top + RoundedQuotient(miniWidth*sourceHeight, sourceWidth) + borderHeight;
        }
        lastCliWidthFixedTo = sourceWidth;
        lastCliHeightFixedTo = sourceHeight;
        PaintMiniView();
    }
    lastUserSetCliWidth = rect->right - rect->left - borderWidth;
    lastUserSetCliHeight = rect->bottom - rect->top - borderHeight;
}

void MiniView::SizeFinished()
{
    if ( this->clipped && this->isGraphicsCaptureCompatible )
    {
        RECT sourceClientRect = {};
        ::GetClientRect(hSource, &sourceClientRect);
        LONG wSrc = sourceClientRect.right - sourceClientRect.left,
             hSrc = sourceClientRect.bottom - sourceClientRect.top;
        this->graphicsCaptureMirror.setClip(rcClip.left, rcClip.top, rcClip.right, rcClip.bottom, wSrc, hSrc);
    }

    if ( user != nullptr )
        user->NotifyChange(*this);
}

void MiniView::WindowMoved()
{
    if ( user != nullptr )
        user->NotifyChange(*this);
}

void MiniView::SetMinMaxSize(MINMAXINFO &minMaxInfo)
{
    minMaxInfo.ptMaxTrackSize.x = 2147483647; // Allow the window to be set to sizes larger than the virtual screen
    minMaxInfo.ptMaxTrackSize.y = 2147483647; // Often necessary to match maximized windows in edit mode

    RECT viewRect = {};
    ::GetClientRect(hSource, &viewRect);
    LONG viewWidth = viewRect.right - viewRect.left,
         viewHeight = viewRect.bottom - viewRect.top;

    if ( viewWidth > viewHeight )
    {
        minMaxInfo.ptMinTrackSize.x = viewHeight == 0 ? 0 : minimumDimension*viewWidth / viewHeight;
        minMaxInfo.ptMinTrackSize.y = minimumDimension;
    }
    else
    {
        minMaxInfo.ptMinTrackSize.x = minimumDimension;
        minMaxInfo.ptMinTrackSize.y = viewWidth == 0 ? 0 : minimumDimension*viewHeight / viewWidth;
    }
}

std::optional<std::string> getWindowText(HWND hWindow)
{
    int titleLength = ::GetWindowTextLength(hWindow) + 1;
    if ( titleLength > 1 )
    {
        std::unique_ptr<icux::codepoint> titleText(new icux::codepoint[titleLength]);
        if ( ::GetWindowText(hWindow, titleText.get(), titleLength) )
        {
            titleText.get()[titleLength - 1] = '\0';
            return icux::toUtf8(icux::uistring(titleText.get(), size_t(titleLength)-1));
        }
    }
    return {};
}

void MiniView::ExitSizeMove()
{
    if ( this->settingWindow ) // Blank MiniView dropped
    {
        POINT pt = {};
        if ( ::GetCursorPos(&pt) == TRUE )
        {
            WindowsItem::Hide();
            HWND newHandle = ::WindowFromPhysicalPoint(pt);
        
            WinGdiImage winGdiImage(newHandle);
            if ( winGdiImage.isValid() )
            {
                this->isGdiCompatible = true;
                this->isGraphicsCaptureCompatible = false;
            }
            else
            {
                newHandle = this->graphicsCaptureMirror.createMirror(WindowsItem::getHandle(), newHandle); // Mirror may need to move to a parent window
                this->isGraphicsCaptureCompatible = true;
                this->isFrozen = false;
            }

            this->SetSourceHandle(newHandle);
            ::GetClientRect(this->hSource, &rcClip);
            this->sourceSize.cx = rcClip.right-rcClip.left;
            this->sourceSize.cy = rcClip.bottom-rcClip.top;
            this->lastUserSetCliWidth = WindowsItem::cliWidth();
            this->lastUserSetCliHeight = WindowsItem::cliHeight();
            this->settingWindow = false;

            auto sourceWindowName = getWindowText(this->hSource);
            if ( auto sourceWindowName = getWindowText(this->hSource) )
                SetWinText("Mini View - [" + (*sourceWindowName) + "]");
            else
                SetWinText("Mini View (Unknown)");

            WindowsItem::Show();

            if ( this->user != nullptr )
            {
                bool matchSourcePos = this->user->GetDefaultMatchSourcePosition(*this);
                bool matchSourceSize = this->user->GetDefaultMatchSourceSize(*this);
                if ( matchSourcePos || matchSourceSize )
                    MatchSource(matchSourcePos, matchSourceSize);

                this->user->NotifyChange(*this);
            }
        }
    }
    else // Size or move finished
        SizeFinished();
}

bool MiniView::IsSourceOnTop()
{
    HWND hForeground = ::GetForegroundWindow();
    if ( hSource == hForeground )
        return true;
    else if ( hSource != NULL && hForeground != NULL )
    {
        HWND hContainer = hSource;
        do
        {
            hContainer = ::GetParent(hContainer);
            if ( hContainer == hForeground )
                return true;
        } while ( hContainer != NULL );
    }
    return false;
}

void MiniView::CheckHideBySourceOnTop()
{
    if ( hideWhenSourceOnTop && hiddenBySourceOnTop != IsSourceOnTop() )
    {
        hiddenBySourceOnTop = !hiddenBySourceOnTop;
        if ( hiddenBySourceOnTop && !editMode )
            Hide();
        else if ( !hiddenBySourceOnTop && !hiddenByParent )
            Show();
    }
}

void MiniView::RunContextMenu()
{
    HMENU hMenu = ::CreatePopupMenu();
    UINT flags = 0;
    if ( !settingWindow )
    {
        if ( isFrozen && (user == nullptr || user->GetShowFrozenContextMenuItem(*this)) )
        {
            flags = MF_BYPOSITION | MF_ENABLED | MF_STRING | MF_UNCHECKED;
            ::InsertMenu(hMenu, (UINT)-1, flags, (UINT_PTR)Id::Frozen, icux::toUistring("Frozen?").c_str());
        }
        flags = MF_BYPOSITION | MF_ENABLED | MF_STRING | (lockRatio ? MF_CHECKED : MF_UNCHECKED);
        ::InsertMenu(hMenu, (UINT)-1, flags, (UINT_PTR)Id::LockRatio, icux::toUistring("Lock Size Ratio").c_str());
        flags = MF_BYPOSITION | MF_ENABLED | MF_STRING | (hideWhenSourceOnTop ? MF_CHECKED : MF_UNCHECKED);
        ::InsertMenu(hMenu, (UINT)-1, flags, (UINT_PTR)Id::HideWhenSourceOnTop, icux::toUistring("Hide When Source Is Top").c_str());
        ::InsertMenu(hMenu, (UINT)-1, MF_BYPOSITION | MF_SEPARATOR, 0, icux::uistring().c_str());
        ::InsertMenu(hMenu, (UINT)-1, MF_BYPOSITION | MF_ENABLED | MF_STRING, (UINT_PTR)Id::MatchSource, icux::toUistring("Match Source").c_str());
        ::InsertMenu(hMenu, (UINT)-1, MF_BYPOSITION | MF_SEPARATOR, 0, icux::uistring().c_str());
        flags = MF_BYPOSITION | MF_STRING | (clipped ? MF_ENABLED : MF_DISABLED);
        ::InsertMenu(hMenu, (UINT)-1, flags, (UINT_PTR)Id::ClearClipRegion, icux::toUistring("Clear Clip Region").c_str());
        ::InsertMenu(hMenu, (UINT)-1, MF_BYPOSITION | MF_ENABLED | MF_STRING, (UINT_PTR)Id::SetClipRegion, icux::toUistring("Set Clip Region").c_str());
        ::InsertMenu(hMenu, (UINT)-1, MF_BYPOSITION | MF_SEPARATOR, 0, icux::uistring().c_str());
        //::InsertMenu(hMenu, (UINT)-1, MF_BYPOSITION | MF_ENABLED | MF_STRING, (UINT_PTR)Id::Hide, icux::toUistring("Hide").c_str());
    }
    ::InsertMenu(hMenu, (UINT)-1, MF_BYPOSITION | MF_ENABLED | MF_STRING, (UINT_PTR)Id::Close, icux::toUistring("Close").c_str());
    //::InsertMenu(hMenu, (UINT)-1, MF_BYPOSITION | MF_ENABLED | MF_STRING, (UINT_PTR)Id::Properties, icux::toUistring("Properties").c_str());

    POINT pt = {};
    ::GetCursorPos(&pt);
    ::SetForegroundWindow(getHandle());
    flags = TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD | TPM_LEFTBUTTON;
    BOOL result = ::TrackPopupMenu(hMenu, flags, pt.x, pt.y, 0, getHandle(), NULL);
    ::PostMessage(getHandle(), WM_NULL, 0, 0);
    ::DestroyMenu(hMenu);

    bool performedCommand = true;
    switch ( result )
    {
        case (BOOL)Id::Frozen: FrozenInfo(); break;
        case (BOOL)Id::ClearClipRegion: ClearClipRegion(); break;
        case (BOOL)Id::SetClipRegion: SetClipRegion(); break;
        case (BOOL)Id::MatchSource: MatchSource(true, true); break;
        case (BOOL)Id::LockRatio: ToggleLockRatio(); break;
        case (BOOL)Id::HideWhenSourceOnTop: ToggleHideWhenSourceOnTop(); break;
        case (BOOL)Id::Hide: HideCommand(); break;
        case (BOOL)Id::Close: PostCloseMessage(); break;
        case (BOOL)Id::Properties: OpenProperties(); break;
        default: performedCommand = false; break;
    }
    if ( performedCommand && user != nullptr )
        user->NotifyChange(*this);
}

void MiniView::PostCloseMessage()
{
    ::PostMessage(getHandle(), WM_CLOSE, NULL, NULL);
}

void MiniView::ProcessCloseMessage()
{
    if ( user != nullptr )
        user->CloseMiniView(*this);
    else
        ClassWindow::DestroyThis();
}

void MiniView::HideCommand()
{
    WinLib::Message("Unimplemented: Hide MiniView");
}

void MiniView::OpenProperties()
{
    WinLib::Message("Unimplemented: Open MiniView Properties");
}

void MiniView::BlackoutMiniView()
{
    RECT rcCli {};
    WindowsItem::getClientRect(rcCli);

    WinLib::DeviceContext dc { getHandle() };
    dc.setBuffer(rcCli);
    dc.fillRect(rcCli, RGB(0, 0, 0));
    dc.flushBuffer();
}

int MiniView::EraseBackground()
{
    if ( settingWindow )
        PaintInstructions();
    else if ( this->isGdiCompatible )
        PaintMiniView();
    else
        BlackoutMiniView();

    return 1;
}

LRESULT MiniView::NonClientHitTest(WPARAM wParam, LPARAM lParam)
{
    LRESULT hit = ClassWindow::WndProc(getHandle(), WM_NCHITTEST, wParam, lParam);
    if ( hit == HTCLIENT && editMode ) // If clicking the client area should result in a grab
        return HTCAPTION; // Allow window to be dragged as if by the title bar
    else
        return hit;
}

LRESULT MiniView::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        case WM_EXITSIZEMOVE: ExitSizeMove(); break;
        case WM_CLOSE: ProcessCloseMessage(); break;
        case WM_ERASEBKGND: return EraseBackground(); break;
        case WM_SIZING: DoSizing(wParam, (RECT*)lParam); return TRUE; break;
        case WM_MOVE: WindowMoved(); break;
        case WM_NCRBUTTONUP: RunContextMenu(); break;
        case WM_GETMINMAXINFO: SetMinMaxSize(*(MINMAXINFO*)lParam); break;
        case WM_NCHITTEST: return NonClientHitTest(wParam, lParam); break;
        default: return ClassWindow::WndProc(hWnd, msg, wParam, lParam); break;
    }
    return 0;
}

WinImage::~WinImage()
{

}

LONG WinImage::getWidth() const
{
    return width;
}

LONG WinImage::getHeight() const
{
    return height;
}

void WinImage::setDimensions(LONG width, LONG height)
{
    this->width = width;
    this->height = height;
}

WinGdiImage::WinGdiImage(WinLib::WindowsItem & windowsItem) : WinImage(), dc(nullptr), bitmapInfo({})
{
    auto width = windowsItem.cliWidth(),
         height = windowsItem.cliHeight();

    if ( width > 0 && height > 0 )
    {
        RECT rcCli {};
        windowsItem.getClientRect(rcCli);

        dc = std::make_unique<WinLib::DeviceContext>(windowsItem.getHandle(), rcCli);
        if ( *dc && dc->copySelfToBuffer() )
        {
            this->setDimensions(rcCli.right-rcCli.left, rcCli.bottom-rcCli.top);
            this->SetBmi();
        }
    }
}

WinGdiImage::WinGdiImage(HWND hSource) : WinImage(), dc(nullptr), bitmapInfo({})
{
    if ( hSource != NULL ) {

        RECT rcSource = {};
        ::GetClientRect(hSource, &rcSource);
        auto sourceWidth = rcSource.right - rcSource.left,
             sourceHeight = rcSource.bottom - rcSource.top;

        if ( sourceWidth > 0 && sourceHeight > 0 )
        {
            dc = std::make_unique<WinLib::DeviceContext>(hSource, rcSource);
            if ( *dc && dc->copySelfToBuffer() )
            {
                this->setDimensions(sourceWidth, sourceHeight);
                this->SetBmi();
            }
        }
    }
}

WinGdiImage::~WinGdiImage()
{

}

bool WinGdiImage::isValid()
{
    auto width = getWidth(),
         height = getHeight();

    if ( width > 0 && height > 0 )
    {
        std::vector<uint32_t> pixels(size_t(width)*size_t(height), uint32_t(0));
        auto linesRetrieved = dc->getDiBits(0, height, &pixels[0], &bitmapInfo);
        return linesRetrieved == height && !isAllZero(pixels);
    }
    else
        return false;
}

bool WinGdiImage::stretchBlt(const WinLib::DeviceContext & dest,
    int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc, int wSrc, int hSrc, DWORD rop)
{
    return dc->stretchBlt(dest, xDest, yDest, wDest, hDest, xSrc, ySrc, wSrc, hSrc, rop);
}

void WinGdiImage::SetBmi()
{
    auto width = getWidth(),
         height = getHeight();

    if ( width > 0 && height > 0 )
    {
        this->bitmapInfo = {};
        BITMAPINFOHEADER & bmiH = this->bitmapInfo.bmiHeader;
        bmiH.biSize = sizeof(BITMAPINFOHEADER);
        bmiH.biWidth = width;
        bmiH.biHeight = -height;
        bmiH.biPlanes = 1;
        bmiH.biBitCount = 32;
        bmiH.biCompression = BI_RGB;
        bmiH.biXPelsPerMeter = 1;
        bmiH.biYPelsPerMeter = 1;
    }
}
