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
	ButtonClearSavedSettings,
};

AdvancedWindow::AdvancedWindow() : blockEditNotify(true), lastActionClearedSettings(false)
{

}

bool AdvancedWindow::CreateThis(HWND hParent, u32 windowId)
{
	if ( getHandle() != NULL )
		return SetParent(hParent);

	RECT rcCli = {};
	if ( GetClientRect(hParent, &rcCli) &&
		ClassWindow::RegisterWindowClass(NULL, NULL, NULL, NULL, NULL, "AdvancedWindow", NULL, false) &&
		ClassWindow::CreateClassWindow(NULL, "Advanced Tab", WS_CHILD,
			5, 22, rcCli.right - rcCli.left-5, rcCli.bottom - rcCli.top-22, hParent, (HMENU)windowId) )
	{
        blockEditNotify = true;
		CreateSubWindows();
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
    blockEditNotify = false;
}

void AdvancedWindow::FixPositions()
{
	
}

bool AdvancedWindow::LastActionClearedSettings()
{
    return lastActionClearedSettings;
}

void AdvancedWindow::CreateSubWindows()
{
	checkShowDialogOnStart.CreateThis(getHandle(), 0, 5, 180, 23,
        miniViews.prefs.ShowDialogOnStart.Get(), "Always Show Dialog On Start", (u32)Id::CheckShowDialogOnStart);
	checkUseNotificationIcon.CreateThis(getHandle(), 0, checkShowDialogOnStart.Bottom() + 2, 180, 23,
        miniViews.prefs.UseNotificationIcon.Get(), "Use Notification Icon", (u32)Id::CheckUseNotificationIcon);
	checkMinimizeToNotificationIcon.CreateThis(getHandle(), 0, checkUseNotificationIcon.Bottom() + 2, 180, 23,
        miniViews.prefs.MinimizeToNotificationIcon.Get(), "Minimize To Notification Icon", (u32)Id::CheckMinimizeToNotificationIcon);
	checkConfirmExit.CreateThis(getHandle(), 0, checkMinimizeToNotificationIcon.Bottom() + 2, 180, 23,
        miniViews.prefs.ConfirmExit.Get(), "Confirm Exit By Dialog", (u32)Id::CheckConfirmExit);
    
    WinLib::TextControl textDefaultOpacity;
    textDefaultOpacity.CreateThis(getHandle(), 0, checkConfirmExit.Bottom() + 8, 147, 23, "Default Opacity Level (0-255): ", 0);
    editDefaultOpacity.CreateThis(getHandle(), textDefaultOpacity.Right() + 5, textDefaultOpacity.Top() - 3, 75, 23, false, (u32)Id::EditDefaultOpacity);
    editDefaultOpacity.SetTextLimit(3);
    editDefaultOpacity.SetEditNum(miniViews.prefs.DefaultOpacity.Get());
    HWND hTransparencyBuddy = CreateWindowEx(NULL, UPDOWN_CLASS, NULL, WS_CHILDWINDOW | WS_VISIBLE | WS_DISABLED | UDS_SETBUDDYINT |
        UDS_ALIGNRIGHT | UDS_ARROWKEYS | UDS_HOTTRACK, 0, 0, 0, 0, getHandle(), (HMENU)Id::EditDefaultOpacityBuddy, NULL, NULL);
    SendMessage(hTransparencyBuddy, UDM_SETBUDDY, (WPARAM)editDefaultOpacity.getHandle(), NULL);
    SendMessage(hTransparencyBuddy, UDM_SETRANGE32, 0, MAKELPARAM(255, 0));
    EnableWindow(hTransparencyBuddy, TRUE);

    groupViewDefaults.CreateThis(getHandle(), 0, editDefaultOpacity.Bottom()+30, 250, 179, "Mini View Defaults", 0);
    checkDefaultMatchSourcePosition.CreateThis(getHandle(), groupViewDefaults.Left() + 10, groupViewDefaults.Top() + 20, 200, 23, false, "Match Source Position by Default", (u32)Id::CheckDefaultMatchSourcePosition);
    checkDefaultMatchSourceSize.CreateThis(getHandle(), groupViewDefaults.Left() + 10, checkDefaultMatchSourcePosition.Bottom() + 2, 200, 23, false, "Match Source Size by Default", (u32)Id::CheckDefaultMatchSourceSize);
    checkDefaultLockSizeRatio.CreateThis(getHandle(), groupViewDefaults.Left() + 10, checkDefaultMatchSourceSize.Bottom() + 2, 200, 23, true, "Lock Size Ratio by Default", (u32)Id::CheckDefaultLockSizeRatio);
    checkDefaultHideWhenSourceOnTop.CreateThis(getHandle(), groupViewDefaults.Left() + 10, checkDefaultLockSizeRatio.Bottom() + 2, 200, 23, true, "Hide When Source on Top by Default", (u32)Id::CheckDefaultHideWhenSourceOnTop);

    buttonClearSavedSettings.CreateThis(getHandle(), 0, groupViewDefaults.Bottom() + 30, 125, 23, "Clear Saved Settings", (u32)Id::ButtonClearSavedSettings);
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
        case Id::ButtonClearSavedSettings: success = miniViews.prefs.ClearSavedPreferences(); break;
		default: performedCommand = false; break;
	}
	
	if ( performedCommand )
	{
        lastActionClearedSettings = (Id)idFrom == Id::ButtonClearSavedSettings;
		if ( !success )
		{
			MessageBox(NULL, std::string("Failed to update settings: " + std::to_string(GetLastError())).c_str(),
				"Error!", MB_OK | MB_ICONEXCLAMATION);
		}
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
