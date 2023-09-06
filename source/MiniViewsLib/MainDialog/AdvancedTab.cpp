#include "AdvancedTab.h"
#include "../Preferences.h"
#include "../MiniViews.h"

enum class Id : int
{
    CheckShowDialogOnStart = ID_FIRST,
    CheckUseNotificationIcon,
    CheckMinimizeToNotificationIcon,
    CheckConfirmExit,
    EditDefaultOpacity,
    EditDefaultOpacityBuddy,
    CheckDefaultMatchSourcePosition,
    CheckDefaultMatchSourceSize,
    CheckDefaultLockSizeRatio,
    CheckDefaultHideWhenSourceOnTop,
    CheckUseCachedImageWhenFrozen,
    CheckShowFrozenIndicatorIcon,
    CheckShowFrozenContextMenuItem,
    ButtonClearSavedSettings
};

AdvancedWindow::AdvancedWindow() : blockEditNotify(true), lastActionClearedSettings(false)
{

}

bool AdvancedWindow::CreateThis(HWND hParent, u64 windowId, int dpi, HFONT font)
{
    if ( getHandle() != NULL )
        return SetParent(hParent);

    RECT rcCli = {};
    if ( GetClientRect(hParent, &rcCli) &&
        ClassWindow::RegisterWindowClass(NULL, NULL, NULL, NULL, NULL, "AdvancedWindow", NULL, false) &&
        ClassWindow::CreateClassWindow(NULL, "Advanced Tab", WS_CHILD,
            DpiScale(5, dpi), DpiScale(22, dpi), rcCli.right - rcCli.left-DpiScale(5, dpi), rcCli.bottom - rcCli.top-DpiScale(22, dpi), hParent, (HMENU)windowId) )
    {
        blockEditNotify = true;
        CreateSubWindows(dpi, font);
        blockEditNotify = false;
        return true;
    }
    else
        return false;
}

bool AdvancedWindow::DestroyThis()
{
    ClassWindow::DestroyThis();
    return true;
}

void AdvancedWindow::RefreshWindow()
{
    blockEditNotify = true;
    checkShowDialogOnStart.SetCheck(miniViews.prefs.ShowDialogOnStart.Get());

    bool useNotificationIcon = miniViews.prefs.UseNotificationIcon.Get();
    checkUseNotificationIcon.SetCheck(useNotificationIcon);
    if ( useNotificationIcon )
    {
        checkMinimizeToNotificationIcon.SetCheck(miniViews.prefs.MinimizeToNotificationIcon.Get());
        checkMinimizeToNotificationIcon.EnableThis();
    }
    else
    {
        checkMinimizeToNotificationIcon.SetCheck(false);
        checkMinimizeToNotificationIcon.DisableThis();
    }
    checkConfirmExit.SetCheck(miniViews.prefs.ConfirmExit.Get());

    editDefaultOpacity.SetEditNum<u32>(miniViews.prefs.DefaultOpacity.Get());

    checkDefaultMatchSourcePosition.SetCheck(miniViews.prefs.DefaultMatchSourcePosition.Get());
    checkDefaultMatchSourceSize.SetCheck(miniViews.prefs.DefaultMatchSourceSize.Get());
    checkDefaultLockSizeRatio.SetCheck(miniViews.prefs.DefaultLockSizeRatio.Get());
    checkDefaultHideWhenSourceOnTop.SetCheck(miniViews.prefs.DefaultHideWhenSourceOnTop.Get());
    checkUseCachedImageWhenFrozen.SetCheck(miniViews.prefs.UseCachedImageWhenFrozen.Get());
    checkShowFrozenIndicatorIcon.SetCheck(miniViews.prefs.ShowFrozenIndicatorIcon.Get());
    checkShowFrozenContextMenuItem.SetCheck(miniViews.prefs.ShowFrozenContextMenuItem.Get());
    blockEditNotify = false;
}

void AdvancedWindow::FixPositions(int dpi, HFONT font)
{
    
}

bool AdvancedWindow::LastActionClearedSettings()
{
    return lastActionClearedSettings;
}

void AdvancedWindow::CreateSubWindows(int dpi, HFONT font)
{
    WindowsItem::setFont(font, false);
    checkShowDialogOnStart.CreateThis(getHandle(), 0, DpiScale(5, dpi), DpiScale(180, dpi), DpiScale(23, dpi),
        miniViews.prefs.ShowDialogOnStart.Get(), "Always Show Dialog On Start", (u32)Id::CheckShowDialogOnStart);
    checkUseNotificationIcon.CreateThis(getHandle(), 0, checkShowDialogOnStart.Bottom() + DpiScale(2, dpi), DpiScale(180, dpi), DpiScale(23, dpi),
        miniViews.prefs.UseNotificationIcon.Get(), "Use Notification Icon", (u32)Id::CheckUseNotificationIcon);
    checkMinimizeToNotificationIcon.CreateThis(getHandle(), 0, checkUseNotificationIcon.Bottom() + DpiScale(2, dpi), DpiScale(180, dpi), DpiScale(23, dpi),
        miniViews.prefs.MinimizeToNotificationIcon.Get(), "Minimize To Notification Icon", (u32)Id::CheckMinimizeToNotificationIcon);
    checkConfirmExit.CreateThis(getHandle(), 0, checkMinimizeToNotificationIcon.Bottom() + DpiScale(2, dpi), DpiScale(180, dpi), DpiScale(23, dpi),
        miniViews.prefs.ConfirmExit.Get(), "Confirm Exit By Dialog", (u32)Id::CheckConfirmExit);
    
    WinLib::TextControl textDefaultOpacity;
    textDefaultOpacity.CreateThis(getHandle(), 0, checkConfirmExit.Bottom() + DpiScale(8, dpi), DpiScale(150, dpi), DpiScale(23, dpi), "Default Opacity Level (0-255): ", 0);
    editDefaultOpacity.CreateThis(getHandle(), textDefaultOpacity.Right() + DpiScale(5, dpi), textDefaultOpacity.Top() - DpiScale(3, dpi), DpiScale(75, dpi), DpiScale(23, dpi), false, (u32)Id::EditDefaultOpacity);
    editDefaultOpacity.SetTextLimit(3);
    editDefaultOpacity.SetEditNum(miniViews.prefs.DefaultOpacity.Get());
    HWND hTransparencyBuddy = CreateWindowEx(NULL, UPDOWN_CLASS, NULL, WS_CHILDWINDOW | WS_VISIBLE | WS_DISABLED | UDS_SETBUDDYINT |
        UDS_ALIGNRIGHT | UDS_ARROWKEYS | UDS_HOTTRACK, 0, 0, 0, 0, getHandle(), (HMENU)Id::EditDefaultOpacityBuddy, NULL, NULL);
    SendMessage(hTransparencyBuddy, UDM_SETBUDDY, (WPARAM)editDefaultOpacity.getHandle(), NULL);
    SendMessage(hTransparencyBuddy, UDM_SETRANGE32, 0, MAKELPARAM(255, 0));
    EnableWindow(hTransparencyBuddy, TRUE);

    groupViewDefaults.CreateThis(getHandle(), 0, editDefaultOpacity.Bottom()+DpiScale(10, dpi), DpiScale(250, dpi), DpiScale(120, dpi), "Mini View Defaults", 0);
    checkDefaultMatchSourcePosition.CreateThis(getHandle(), groupViewDefaults.Left() + DpiScale(10, dpi), groupViewDefaults.Top() + DpiScale(20, dpi), DpiScale(200, dpi), DpiScale(23, dpi), false, "Match Source Position by Default", (u32)Id::CheckDefaultMatchSourcePosition);
    checkDefaultMatchSourceSize.CreateThis(getHandle(), groupViewDefaults.Left() + DpiScale(10, dpi), checkDefaultMatchSourcePosition.Bottom() + DpiScale(2, dpi), DpiScale(200, dpi), DpiScale(23, dpi), false, "Match Source Size by Default", (u32)Id::CheckDefaultMatchSourceSize);
    checkDefaultLockSizeRatio.CreateThis(getHandle(), groupViewDefaults.Left() + DpiScale(10, dpi), checkDefaultMatchSourceSize.Bottom() + DpiScale(2, dpi), DpiScale(200, dpi), DpiScale(23, dpi), true, "Lock Size Ratio by Default", (u32)Id::CheckDefaultLockSizeRatio);
    checkDefaultHideWhenSourceOnTop.CreateThis(getHandle(), groupViewDefaults.Left() + DpiScale(10, dpi), checkDefaultLockSizeRatio.Bottom() + DpiScale(2, dpi), DpiScale(200, dpi), DpiScale(23, dpi), true, "Hide When Source on Top by Default", (u32)Id::CheckDefaultHideWhenSourceOnTop);

    groupFrozenSourceBehaviorDefaults.CreateThis(getHandle(), 0, groupViewDefaults.Bottom()+DpiScale(10, dpi), DpiScale(250, dpi), DpiScale(95, dpi), "Frozen Source Behavior", 0);
    checkUseCachedImageWhenFrozen.CreateThis(getHandle(), groupFrozenSourceBehaviorDefaults.Left() + DpiScale(10, dpi), groupFrozenSourceBehaviorDefaults.Top() + DpiScale(20, dpi), DpiScale(200, dpi), DpiScale(23, dpi), true, "Use Cached Image When Frozen", (u32)Id::CheckUseCachedImageWhenFrozen);
    checkShowFrozenIndicatorIcon.CreateThis(getHandle(), groupFrozenSourceBehaviorDefaults.Left() + DpiScale(10, dpi), checkUseCachedImageWhenFrozen.Bottom() + DpiScale(2, dpi), DpiScale(200, dpi), DpiScale(23, dpi), true, "Show Frozen Indicator Icon", (u32)Id::CheckShowFrozenIndicatorIcon);
    checkShowFrozenContextMenuItem.CreateThis(getHandle(), groupFrozenSourceBehaviorDefaults.Left() + DpiScale(10, dpi), checkShowFrozenIndicatorIcon.Bottom() + DpiScale(2, dpi), DpiScale(200, dpi), DpiScale(23, dpi), true, "Show Frozen Context Menu Item", (u32)Id::CheckShowFrozenContextMenuItem);

    buttonClearSavedSettings.CreateThis(getHandle(), 0, groupFrozenSourceBehaviorDefaults.Bottom() + DpiScale(8, dpi), DpiScale(125, dpi), DpiScale(23, dpi), "Clear Saved Settings", (u32)Id::ButtonClearSavedSettings);
}

bool ToggleUseNotificationIcon()
{
    if ( miniViews.prefs.UseNotificationIcon.Toggle() )
    {
        if ( miniViews.prefs.UseNotificationIcon.Get() )
            miniViews.CreateNotificationIcon();
        else
            miniViews.RemoveNotificationIcon();

        return true;
    }
    return false;
}

void AdvancedWindow::NotifyButtonClicked(int idFrom, HWND)
{
    bool success = true;
    bool performedCommand = true;
    switch ( (Id)idFrom )
    {
        case Id::CheckUseNotificationIcon: success = miniViews.prefs.UseNotificationIcon.Toggle(); miniViews.RemoveNotificationIcon(); break;
        case Id::CheckShowDialogOnStart: success = miniViews.prefs.ShowDialogOnStart.Toggle(); break;
        case Id::CheckMinimizeToNotificationIcon: success = miniViews.prefs.MinimizeToNotificationIcon.Toggle(); break;
        case Id::CheckConfirmExit: success = miniViews.prefs.ConfirmExit.Toggle(); break;
        case Id::CheckDefaultMatchSourcePosition: success = miniViews.prefs.DefaultMatchSourcePosition.Toggle(); break;
        case Id::CheckDefaultMatchSourceSize: success = miniViews.prefs.DefaultMatchSourceSize.Toggle(); break;
        case Id::CheckDefaultLockSizeRatio: success = miniViews.prefs.DefaultLockSizeRatio.Toggle(); break;
        case Id::CheckDefaultHideWhenSourceOnTop: success = miniViews.prefs.DefaultHideWhenSourceOnTop.Toggle(); break;
        case Id::CheckUseCachedImageWhenFrozen: success = miniViews.prefs.UseCachedImageWhenFrozen.Toggle(); break;
        case Id::CheckShowFrozenIndicatorIcon: success = miniViews.prefs.ShowFrozenIndicatorIcon.Toggle(); break;
        case Id::CheckShowFrozenContextMenuItem: success = miniViews.prefs.ShowFrozenContextMenuItem.Toggle(); break;
        case Id::ButtonClearSavedSettings: success = miniViews.prefs.ClearSavedPreferences(); break;
        default: performedCommand = false; break;
    }
    
    if ( performedCommand )
    {
        lastActionClearedSettings = (Id)idFrom == Id::ButtonClearSavedSettings;
        if ( !success )
            WinLib::Message("Failed to update settings: " + std::to_string(GetLastError()), "Error!");

        RefreshWindow();
    }
}

void AdvancedWindow::NotifyEditUpdated(int idFrom, HWND)
{
    if ( !blockEditNotify )
    {
        if ( idFrom == (int)Id::EditDefaultOpacity )
        {
            int newOpacity = 0;
            if ( editDefaultOpacity.GetEditNum<int>(newOpacity) )
            {
                if ( newOpacity > 255 )
                    newOpacity = 255;
                else if ( newOpacity < 0 )
                    newOpacity = 0;

                miniViews.prefs.DefaultOpacity.Set((u32)newOpacity);
            }
            else if ( editDefaultOpacity.GetTextLength() != 0 )
                editDefaultOpacity.SetEditNum(miniViews.prefs.DefaultOpacity.Get());
        }
    }
}
