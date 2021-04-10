#include "MiniViews.h"
#include "Preferences.h"
#include "Resource.h"
#include "Version.h"
#include <ShellScalingApi.h>

enum class Msg : UINT
{
	ProcessFrame = MSG_FIRST,
	OpenDialog,
	IconNotify,
};

enum class Id : int
{
	ToggleEditMode = ID_FIRST,
	AddMiniView,
	ClearMiniViews,
	IncreaseOpacity,
	DecreaseOpacity,
	ExitApplication
};

enum class Hotkey : int
{
	ToggleEditMode = 1,
	AddMiniView,
	IncreaseOpacity,
	DecreaseOpacity,
};

const int MiniViews::msPerFrame(25);

const int MiniViews::framesPerCycle(500/MiniViews::msPerFrame);

const u32 MiniViews::notificationId(0x10C9AAD6);

const std::string MiniViews::mainClassName("wcMiniViews");

const std::string MiniViews::mainWindowName(std::string("Mini Views ") + GetShortVersionString());

MiniViews::MiniViews() : prefs(RegistryKey(HKEY_CURRENT_USER, "SOFTWARE\\Mini Views")),
    defaultCursor(NULL), smallIcon(NULL), mediumIcon(NULL), transparentColor(RGB(255, 178, 255)),
	transparentBrush(NULL), transparencyAlpha(175), editMode(true), hasAutoDisplayedPage(false),
	frameNumber(0), hasFrameToRun(false), updater(*this)
{
    transparencyAlpha = (u8)prefs.DefaultOpacity.Get();
	defaultCursor = ::LoadCursor(NULL, IDC_ARROW);
	transparentBrush = ::CreateSolidBrush(transparentColor);
	smallIcon = (HICON)::LoadImage(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MINIVIEWSICON), IMAGE_ICON, 16, 16, 0);
	mediumIcon = (HICON)::LoadImage(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MINIVIEWSICON), IMAGE_ICON, 32, 32, 0);
}

MiniViews::~MiniViews()
{
	::DestroyCursor(defaultCursor);
	::DestroyIcon(smallIcon);
	::DestroyIcon(mediumIcon);
}

int MiniViews::Run(int showCommand)
{
	SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
	RefreshNotificationIconArea();

	if ( !EnsureOnlyInstance() )
		return 1;

	if ( !CreateThis() )
		return CriticalError(2, "Window creation failed.");

	if ( prefs.UseNotificationIcon.Get() && !CreateNotificationIcon() )
		MessageBox(NULL, "Failed to create notification icon.", "Error!", MB_OK);

	if ( !RegisterHotkeys() )
		MessageBox(NULL, "One or more hotkeys failed to register.", "Error!", MB_OK);

	::ShowWindow(getHandle(), showCommand);
	WindowsItem::UpdateWindow();

	if ( prefs.ShowDialogOnStart.Get() || !prefs.UseNotificationIcon.Get() )
		mainDialog.CreateThis(NULL);

	MSG msg = { };
	bool keepRunning = true;
	do
	{
        // You must call PeekMessage for any additional top-level windows to prevent them from hanging
		while ( ::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0 ||
                ::PeekMessage(&msg, mainDialog.getHandle(), 0, 0, PM_REMOVE) != 0 )
		{
			if ( msg.message == WM_QUIT )
				keepRunning = false;
			else
            {
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
		}

		if ( hasFrameToRun )
			RunFrame();
		
		std::this_thread::sleep_for(std::chrono::milliseconds(1)); // Avoid consuming a core
	}
	while ( keepRunning );

	ClearMiniViews();
	updater.StopTimedUpdates();
	RemoveNotificationIcon();

    if ( !mainDialog.advancedWindow.LastActionClearedSettings() )
        prefs.SetNotFirstRun();

	return msg.wParam;
}

int MiniViews::CriticalError(int returnValue, const char* errorMessage)
{
	::MessageBox(NULL, errorMessage, "Mini Views Critical Error", MB_OK | MB_ICONEXCLAMATION);
	return returnValue;
}

bool MiniViews::EnsureOnlyInstance()
{
	HWND hOtherInstance = ::FindWindow(mainClassName.c_str(), mainWindowName.c_str());
	if ( hOtherInstance != NULL )
		::PostMessage(hOtherInstance, (UINT)Msg::OpenDialog, NULL, NULL);

	return hOtherInstance == NULL;
}

void MiniViews::RunFrame()
{
	// Update frame information
	frameNumber++;
	if ( frameNumber >= framesPerCycle )
		frameNumber = 0;

	// Actions that occur every cycle
	if ( frameNumber == 0 )
		RunHalfSecondActions();

	// Actions that occur every frame
	for ( auto &miniView : currMiniViews )
		miniView->RunFrame();

	// Allow the Updater thread to resume
	hasFrameToRun = false;
}

void MiniViews::RunHalfSecondActions()
{
	FixPosition();
	if ( prefs.UseNotificationIcon.Get() )
		CreateNotificationIcon();
	for ( auto &miniView : currMiniViews )
		miniView->ValidateProperties();
}

void MiniViews::TimedUpdate()
{
	hasFrameToRun = true;
}

std::vector<std::shared_ptr<MiniView>> &MiniViews::GetMiniViewList()
{
	return currMiniViews;
}

void MiniViews::Rest()
{
    ::SetWindowPos(getHandle(), HWND_BOTTOM, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
}

void MiniViews::Wake()
{
    FixPosition();
}

void MiniViews::NotifyChange(MiniView &)
{
	mainDialog.RefreshViewsTab(false, true);
}

void MiniViews::CloseMiniView(MiniView &miniView)
{
	HWND hMiniView = miniView.getHandle();
	auto toDelete = currMiniViews.end();
	for ( auto it = currMiniViews.begin(); it != currMiniViews.end(); ++it )
	{
		if ( hMiniView == (*it)->getHandle() )
		{
			toDelete = it;
			break;
		}
	}

	miniView.DestroyThis();

	if ( toDelete != currMiniViews.end() )
		currMiniViews.erase(toDelete);

    if ( currMiniViews.size() == 0 )
        Rest();

	mainDialog.RefreshViewsTab(true, true);
}

void MiniViews::SetClipRegion(MiniView &miniView)
{
	if ( !editMode )
		ToggleEditMode();

	::SetForegroundWindow(miniView.GetSourceWindow());
	::SetLayeredWindowAttributes(getHandle(), transparentColor, 175, LWA_COLORKEY | LWA_ALPHA);

	for ( auto &toHide : currMiniViews )
		toHide->HideByParentRequest();

	clipSel.CreateThis(getHandle(), miniView);
}

bool MiniViews::GetDefaultMatchSourcePosition(MiniView &)
{
    return prefs.DefaultMatchSourcePosition.Get();
}

bool MiniViews::GetDefaultMatchSourceSize(MiniView &)
{
    return prefs.DefaultMatchSourceSize.Get();
}

bool MiniViews::GetDefaultLockSizeRatio(MiniView &)
{
    return prefs.DefaultLockSizeRatio.Get();
}

bool MiniViews::GetDefaultHideWhenSourceOnTop(MiniView &)
{
    return prefs.DefaultHideWhenSourceOnTop.Get();
}

void MiniViews::EndSetClipRegion(MiniView &miniView, RECT rcClip)
{
	clipSel.DestroyThis();

	for ( auto &toShow : currMiniViews )
		toShow->ShowByParentRequest();

	::SetLayeredWindowAttributes(getHandle(), transparentColor, transparencyAlpha, LWA_COLORKEY | LWA_ALPHA);
	miniView.SetClip(true, rcClip.left, rcClip.top, rcClip.right, rcClip.bottom);
}

bool MiniViews::IsInEditMode()
{
	return editMode;
}

void MiniViews::ToggleEditMode()
{
	editMode = !editMode;

	if ( editMode )
		WindowsItem::SetWinLong(GWL_EXSTYLE, WindowsItem::GetWinLong(GWL_EXSTYLE)&(~WS_EX_TRANSPARENT));
	else
		WindowsItem::SetWinLong(GWL_EXSTYLE, WindowsItem::GetWinLong(GWL_EXSTYLE) | WS_EX_TRANSPARENT);

	for ( auto &miniView : currMiniViews )
		miniView->SetEditMode(editMode);

	mainDialog.generalWindow.RefreshWindow();
}

void MiniViews::AddMiniView()
{
    Wake();

	if ( editMode == false )
		ToggleEditMode();

	WindowsItem::SetWinLong(GWL_EXSTYLE, WindowsItem::GetWinLong(GWL_EXSTYLE)&(~WS_EX_TRANSPARENT));
	std::shared_ptr<MiniView> newMiniView = std::shared_ptr<MiniView>(new MiniView(this));

	int miniViewXc = 50 + 4 * currMiniViews.size(),
		miniViewYc = 70 + 4 * currMiniViews.size();

	POINT cursor = {};
	if ( GetCursorPos(&cursor) != 0 && ScreenToClient(getHandle(), &cursor) != 0 )
	{
		miniViewXc = cursor.x;
		miniViewYc = cursor.y;
	}

	if ( newMiniView->CreateThis(WindowsItem::getHandle(), ::GetDesktopWindow(), miniViewXc, miniViewYc) )
		currMiniViews.push_back(newMiniView);

	mainDialog.RefreshViewsTab(true, true);
}

void MiniViews::ClearMiniViews()
{
	for ( auto miniView : currMiniViews )
		miniView->DestroyThis();

	currMiniViews.clear();
}

u8 MiniViews::GetOpacityLevel()
{
	return transparencyAlpha;
}

void MiniViews::SetOpacityLevel(u8 newOpacityLevel)
{
	transparencyAlpha = newOpacityLevel;
	::SetLayeredWindowAttributes(WindowsItem::getHandle(), transparentColor, transparencyAlpha, LWA_COLORKEY | LWA_ALPHA);
	mainDialog.generalWindow.RefreshWindow();
}

bool MiniViews::CreateNotificationIcon()
{
    if ( HasNotificationIcon() )
        return true;

    NOTIFYICONDATA notifyIconData = {};
    notifyIconData.cbSize = sizeof(NOTIFYICONDATA);
    notifyIconData.hWnd = getHandle();
    notifyIconData.uID = notificationId;
    notifyIconData.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    notifyIconData.uCallbackMessage = (UINT)Msg::IconNotify;
    notifyIconData.hIcon = smallIcon;
    std::strcpy(notifyIconData.szTip, mainWindowName.c_str());
    notifyIconData.hBalloonIcon = smallIcon;
    return ::Shell_NotifyIcon(NIM_ADD, &notifyIconData) == TRUE;
}

void MiniViews::RemoveNotificationIcon()
{
	NOTIFYICONDATA notifyIconData = {};
	notifyIconData.cbSize = sizeof(NOTIFYICONDATA);
	notifyIconData.uID = notificationId;
	notifyIconData.hWnd = getHandle();
	::Shell_NotifyIcon(NIM_DELETE, &notifyIconData);
}

bool MiniViews::CreateThis()
{
	if ( ClassWindow::RegisterWindowClass(NULL, mediumIcon, defaultCursor, transparentBrush, NULL, mainClassName.c_str(),
										  smallIcon, false) &&
		 ClassWindow::CreateClassWindow(WS_EX_TOPMOST|WS_EX_TOOLWINDOW, mainWindowName.c_str(), NULL, CW_USEDEFAULT, CW_USEDEFAULT,
										CW_USEDEFAULT, CW_USEDEFAULT, ::GetDesktopWindow(), NULL) )
	{
		if ( !updater.StartTimedUpdates(msPerFrame) )
		{
			::MessageBox(NULL, "Critical Error!", "Could not open graphic updater thread.", MB_OK|MB_ICONEXCLAMATION);
			::PostQuitMessage(0);
		}
		WindowsItem::SetWinLong(GWL_STYLE, 0);
		WindowsItem::SetWinLong(GWL_EXSTYLE, WS_EX_TOPMOST|WS_EX_LAYERED|WS_EX_TOOLWINDOW); // WS_EX_TRANSPARENT for clickthrough;
		::SetWindowPos(getHandle(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
		
		::SetLayeredWindowAttributes(getHandle(), transparentColor, transparencyAlpha, LWA_COLORKEY|LWA_ALPHA);
		WindowsItem::ShowNormal();
        Rest();
		return true;
	}
	return false;
}

bool MiniViews::RegisterHotkeys()
{
	if ( !::RegisterHotKey(getHandle(), (int)Hotkey::ToggleEditMode, MOD_CONTROL|MOD_SHIFT, 'E') ||
		 !::RegisterHotKey(getHandle(), (int)Hotkey::AddMiniView, MOD_CONTROL|MOD_SHIFT, 'A') ||
		 !::RegisterHotKey(getHandle(), (int)Hotkey::IncreaseOpacity, MOD_CONTROL|MOD_SHIFT, VK_OEM_PLUS) ||
		 !::RegisterHotKey(getHandle(), (int)Hotkey::DecreaseOpacity, MOD_CONTROL|MOD_SHIFT, VK_OEM_MINUS) )
	{
		return false;
	}
	else
		return true;
}

bool MiniViews::isTransparencyMinimum()
{
	return transparencyAlpha == 0;
}

bool MiniViews::isTransparencyMax()
{
	return transparencyAlpha == 255;
}

void MiniViews::IncrementAlpha()
{
	if ( (u16)transparencyAlpha + 10 > 255 )
		SetOpacityLevel(255);
	else
		SetOpacityLevel(GetOpacityLevel() + 10);
}

void MiniViews::DecrementAlpha()
{
	if ( (s16)transparencyAlpha - 10 < 0 )
		SetOpacityLevel(0);
	else
		SetOpacityLevel(GetOpacityLevel() - 10);
}

void MiniViews::FixPosition()
{
    int virtualLeft = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int virtualTop = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int virtualWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int virtualHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    RECT windowRect;
    HWND topWindow = GetTopWindow(NULL);
    bool correctZOrder = true;
    do
    {
        if ( topWindow == getHandle() )
            break;
        else if ( topWindow != NULL )
        {
            correctZOrder = (GetWindowLong(topWindow, GWL_EXSTYLE) & WS_EX_TOPMOST) == WS_EX_TOPMOST;
            topWindow = GetNextWindow(topWindow, GW_HWNDNEXT);
        }
    }
    while ( correctZOrder && topWindow != NULL );

    if ( !correctZOrder || getWindowRect(windowRect) &&
            ( windowRect.left != virtualLeft || windowRect.top != virtualTop ||
              windowRect.right - windowRect.left != virtualWidth || windowRect.bottom - windowRect.top != virtualHeight) )
    {
        ::SetWindowPos(getHandle(), HWND_TOPMOST, virtualLeft, virtualTop, virtualWidth, virtualHeight, SWP_NOACTIVATE);
    }
}

bool MiniViews::HasNotificationIcon()
{
	NOTIFYICONIDENTIFIER notifyIconIdentifier = {};
	notifyIconIdentifier.cbSize = sizeof(NOTIFYICONIDENTIFIER);
	notifyIconIdentifier.hWnd = getHandle();
	notifyIconIdentifier.uID = notificationId;
	RECT unused = {};
	return ::Shell_NotifyIconGetRect(&notifyIconIdentifier, &unused) == S_OK;
}

void MiniViews::RunNotificationContextMenu()
{
	HMENU hMenu = ::CreatePopupMenu();
	UINT flags = MF_BYPOSITION | MF_ENABLED | MF_STRING | (editMode ? MF_CHECKED : MF_UNCHECKED);
	::InsertMenu(hMenu, (UINT)-1, flags, (UINT_PTR)Id::ToggleEditMode, "Edit Mode\t(Ctrl Shift E)");
	::InsertMenu(hMenu, (UINT)-1, MF_BYPOSITION | MF_SEPARATOR, 0, "");

	::InsertMenu(hMenu, (UINT)-1, MF_BYPOSITION | MF_ENABLED | MF_STRING, (UINT_PTR)Id::AddMiniView, "Add Mini View\t(Ctrl Shift A)");
	flags = MF_BYPOSITION | MF_ENABLED | MF_STRING | (currMiniViews.size() > 0 ? MF_ENABLED : MF_DISABLED);
	::InsertMenu(hMenu, (UINT)-1, flags, (UINT_PTR)Id::ClearMiniViews, "Clear Mini Views");
	::InsertMenu(hMenu, (UINT)-1, MF_BYPOSITION | MF_SEPARATOR, 0, "");

	flags = MF_BYPOSITION | MF_STRING | (isTransparencyMax() ? MF_DISABLED : MF_ENABLED);
	::InsertMenu(hMenu, (UINT)-1, flags, (UINT_PTR)Id::IncreaseOpacity, "Increase Opacity\t(Ctrl Shift +)");
	flags = MF_BYPOSITION | MF_STRING | (isTransparencyMinimum() ? MF_DISABLED : MF_ENABLED);
	::InsertMenu(hMenu, (UINT)-1, flags, (UINT_PTR)Id::DecreaseOpacity, "Decrease Opacity\t(Ctrl Shift -)");
	::InsertMenu(hMenu, (UINT)-1, MF_BYPOSITION | MF_SEPARATOR, 0, "");

	::InsertMenu(hMenu, (UINT)-1, MF_BYPOSITION | MF_ENABLED | MF_STRING, (UINT_PTR)Id::ExitApplication, "Exit");

	POINT pt = {};
	::GetCursorPos(&pt);
	::SetForegroundWindow(getHandle());
	flags = TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD | TPM_LEFTBUTTON;
	BOOL result = ::TrackPopupMenu(hMenu, flags, pt.x, pt.y, 0, getHandle(), NULL);
	::PostMessage(getHandle(), WM_NULL, 0, 0);
	::DestroyMenu(hMenu);

	switch ( result )
	{
		case (BOOL)Id::ToggleEditMode: ToggleEditMode(); break;
		case (BOOL)Id::AddMiniView: AddMiniView(); break;
		case (BOOL)Id::IncreaseOpacity: IncrementAlpha(); break;
		case (BOOL)Id::DecreaseOpacity: DecrementAlpha(); break;
		case (BOOL)Id::ExitApplication: PostQuitMessage(0); break;
		case (BOOL)Id::ClearMiniViews: ClearMiniViews(); break;
	}
}

LRESULT MiniViews::HotkeyPressed(WPARAM wParam, LPARAM lParam)
{
	switch ( wParam )
	{
		case (WPARAM)Hotkey::ToggleEditMode: ToggleEditMode(); break;
		case (WPARAM)Hotkey::AddMiniView: AddMiniView(); break;
		case (WPARAM)Hotkey::IncreaseOpacity: IncrementAlpha(); break;
		case (WPARAM)Hotkey::DecreaseOpacity: DecrementAlpha(); break;
	}
	return ClassWindow::WndProc(getHandle(), WM_HOTKEY, wParam, lParam);
}

LRESULT MiniViews::IconNotify(WPARAM wParam, LPARAM lParam)
{
	switch ( lParam )
	{
		case WM_RBUTTONUP: RunNotificationContextMenu(); break;
		case WM_LBUTTONDBLCLK: mainDialog.ToggleShown(NULL); break;
	}
	return ClassWindow::WndProc(getHandle(), (UINT)Msg::IconNotify, wParam, lParam);
}

LRESULT MiniViews::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case WM_SIZE: FixPosition(); break;
		case WM_ACTIVATEAPP: return 0; break; // Don't allow default behavior
		case WM_HOTKEY: return HotkeyPressed(wParam, lParam); break;
		case (UINT)Msg::OpenDialog: mainDialog.CreateThis(NULL); break;
		case (UINT)Msg::IconNotify: return IconNotify(wParam, lParam); break;
		default: return ClassWindow::WndProc(hWnd, msg, wParam, lParam); break;
	}
	return 0;
}

void RefreshNotificationIconArea()
{
	RECT rcNotify = {};
	HWND hPromotedNotifyArea = ::FindWindowEx(::FindWindowEx(::FindWindowEx(::FindWindowEx(NULL,
		NULL, "Shell_TrayWnd", ""),
		NULL, "TrayNotifyWnd", ""),
		NULL, "SysPager", ""),
		NULL, "ToolbarWindow32", "User Promoted Notification Area");

	::GetClientRect(hPromotedNotifyArea, &rcNotify);
	for ( int x = 0; x < rcNotify.right; x += 5 )
	{
		for ( int y = 0; y < rcNotify.bottom; y += 5 )
			::SendMessage(hPromotedNotifyArea, WM_MOUSEMOVE, 0, MAKELPARAM(x, y));
	}

	HWND hHiddenNotifyArea = ::FindWindowEx(::FindWindowEx(NULL,
		NULL, "NotifyIconOverflowWindow", ""),
		NULL, "ToolbarWindow32", "Overflow Notification Area");

	::GetClientRect(hHiddenNotifyArea, &rcNotify);
	for ( int x = 0; x < rcNotify.right; x += 5 )
	{
		for ( int y = 0; y < rcNotify.bottom; y += 5 )
			::SendMessage(hHiddenNotifyArea, WM_MOUSEMOVE, 0, MAKELPARAM(x, y));
	}
}