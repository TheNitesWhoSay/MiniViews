#include "MainDialog.h"
#include "../Preferences.h"
#include "../MiniViews.h"
#include "../Resource.h"

enum class Id : int
{
	GeneralWindow = ID_FIRST,
	ViewsWindow,
	AdvancedWindow,
    AboutWindow,
	MainDialogTabs
};

MainDialog::MainDialog() : windowLeft(0), windowTop(0), selectedTab(TabId::GeneralTab), defaultFont(NULL), smallIcon(NULL),
	mediumIcon(NULL)
{
	defaultFont = ::CreateFont(14, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, icux::toUistring("Microsoft Sans Serif").c_str());
	smallIcon = (HICON)::LoadImage(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MINIVIEWSICON), IMAGE_ICON, 16, 16, 0);
	mediumIcon = (HICON)::LoadImage(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MINIVIEWSICON), IMAGE_ICON, 32, 32, 0);
}

MainDialog::~MainDialog()
{
	::DeleteObject(defaultFont);
	::DestroyIcon(smallIcon);
	::DestroyIcon(mediumIcon);
}

bool MainDialog::CreateThis(HWND hParent)
{
    if ( ClassWindow::RegisterWindowClass(NULL, mediumIcon, NULL, GetSysColorBrush(COLOR_WINDOW), NULL, "MainMiniViewsDialog", smallIcon, false) &&
         ClassWindow::CreateClassWindow(WS_EX_APPWINDOW, MiniViews::mainWindowName.c_str(), WS_SYSMENU|WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT,
             279, 464, hParent, NULL) )
    {
        CreateSubWindows();
        WindowsItem::ShowNormal();
        SetForegroundWindow(getHandle());
        return true;
    }
    return false;
}

bool MainDialog::DestroyThis()
{
    generalWindow.DestroyThis();
    viewsWindow.DestroyThis();
    advancedWindow.DestroyThis();
    aboutWindow.DestroyThis();
    tabs.DestroyThis();

	RECT windowRect = {};
	WindowsItem::getWindowRect(windowRect);
	windowLeft = windowRect.left;
	windowTop = windowRect.top;

    if ( WindowsItem::getHandle() != NULL )
        ClassWindow::DestroyThis();

    return true;
}

bool MainDialog::IsShown()
{
	return WindowsItem::getHandle() != NULL;
}

void MainDialog::ToggleShown(HWND hParent)
{
	if ( WindowsItem::getHandle() == NULL )
		CreateThis(hParent);
	else
		DestroyThis();
}

void MainDialog::ChangeTab(u32 tabId)
{
	tabs.SetCurSel(tabId);
	switch ( tabId )
	{
		case (u32)TabId::GeneralTab: generalWindow.RefreshWindow(); break;
		case (u32)TabId::ViewsTab: viewsWindow.RefreshWindow(true, true); break;
		case (u32)TabId::AdvancedTab: advancedWindow.RefreshWindow(); break;
        case (u32)TabId::AboutTab: aboutWindow.RefreshWindow(); break;
	}
}

void MainDialog::RefreshViewsTab(bool rebuildTree, bool refreshNames)
{
	if ( WindowsItem::getHandle() != NULL && selectedTab == TabId::ViewsTab )
		viewsWindow.RefreshWindow(rebuildTree, refreshNames);
}

void MainDialog::FixPositions()
{
	RECT rcCli = {};
	getClientRect(rcCli);
	tabs.SetPos(rcCli.left, rcCli.top, rcCli.right - rcCli.left, rcCli.bottom - rcCli.top);
	tabs.getClientRect(rcCli);

	generalWindow.SetPos(rcCli.left+5, rcCli.top+22, rcCli.right - rcCli.left-5, rcCli.bottom - rcCli.top-22);
	viewsWindow.SetPos(rcCli.left+5, rcCli.top+22, rcCli.right - rcCli.left-5, rcCli.bottom - rcCli.top-22);
	advancedWindow.SetPos(rcCli.left+5, rcCli.top+22, rcCli.right - rcCli.left-5, rcCli.bottom - rcCli.top-22);
    aboutWindow.SetPos(rcCli.left+5, rcCli.top+22, rcCli.right - rcCli.left-5, rcCli.bottom - rcCli.top-22);
	generalWindow.FixPositions();
	viewsWindow.FixPositions();
	advancedWindow.FixPositions();
    aboutWindow.FixPositions();
}

void MainDialog::ProcessClose()
{
	if ( miniViews.prefs.ConfirmExit.Get() )
	{
		if ( WinLib::GetYesNo("Would you like to exit Mini Views?", "Confirm Exit") == WinLib::PromptResult::Yes )
			::PostQuitMessage(0);
	}
	else
		::PostQuitMessage(0);
}

void MainDialog::CreateSubWindows()
{
    WindowsItem::SetSmallIcon(smallIcon);
    WindowsItem::SetMedIcon(mediumIcon);
    //MoveTo(windowLeft, windowTop);

    tabs.CreateThis(getHandle(), 0, 0, 263, 426, (u32)Id::MainDialogTabs);
    //tabs.FindThis(getHandle(), IDC_MAINDIALOGTABS);
	std::vector<std::string> tabLabels = { "General", "Views", "Advanced", "About" };
    for ( size_t i = 0; i < tabLabels.size(); i++ )
        tabs.InsertTab((u32)i, tabLabels[i]);

	generalWindow.CreateThis(tabs.getHandle(), (u32)Id::GeneralWindow);
	viewsWindow.CreateThis(tabs.getHandle(), (u32)Id::ViewsWindow);
	advancedWindow.CreateThis(tabs.getHandle(), (u32)Id::AdvancedWindow);
    aboutWindow.CreateThis(tabs.getHandle(), (u32)Id::AboutWindow);

    ChangeTab((u32)selectedTab);
    switch ( selectedTab )
    {
        case TabId::GeneralTab: generalWindow.Show(); break;
        case TabId::ViewsTab: viewsWindow.Show(); break;
        case TabId::AdvancedTab: advancedWindow.Show(); break;
        case TabId::AboutTab: aboutWindow.Show(); break;
    }
    WindowsItem::ReplaceChildFonts(defaultFont);
    FixPositions();
}

void MainDialog::TabSelChange()
{
	selectedTab = (TabId)tabs.GetCurSel();
	switch ( selectedTab )
	{
		case TabId::GeneralTab:
			tabs.ShowTab((u32)Id::GeneralWindow);
			generalWindow.RefreshWindow();
			break;

		case TabId::ViewsTab:
			tabs.ShowTab((u32)Id::ViewsWindow);
			viewsWindow.RefreshWindow(true, true);
			break;

		case TabId::AdvancedTab:
			tabs.ShowTab((u32)Id::AdvancedWindow);
			advancedWindow.RefreshWindow();
			break;

        case TabId::AboutTab:
            tabs.ShowTab((u32)Id::AboutWindow);
            aboutWindow.RefreshWindow();
            break;
	}
}

void MainDialog::TabSelChanging()
{
	switch ( (TabId)tabs.GetCurSel() )
	{
		case TabId::GeneralTab: tabs.HideTab((u32)Id::GeneralWindow); break;
		case TabId::ViewsTab: tabs.HideTab((u32)Id::ViewsWindow); break;
		case TabId::AdvancedTab: tabs.HideTab((u32)Id::AdvancedWindow); break;
        case TabId::AboutTab: tabs.HideTab((u32)Id::AboutWindow); break;
	}
}

LRESULT MainDialog::SysCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if ( wParam == SC_MINIMIZE && miniViews.prefs.UseNotificationIcon.Get() && miniViews.prefs.MinimizeToNotificationIcon.Get() )
		DestroyThis();
	
	return ClassWindow::WndProc(hWnd, WM_SYSCOMMAND, wParam, lParam);
}

LRESULT MainDialog::Notify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr)
{
	switch ( nmhdr->code )
	{
		case TCN_SELCHANGE: TabSelChange(); break;
		case TCN_SELCHANGING: TabSelChanging(); break;
        default: return ClassWindow::Notify(hWnd, idFrom, nmhdr); break;
	}
	return 0;
}

LRESULT MainDialog::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case WM_SYSCOMMAND: return SysCommand(hWnd, wParam, lParam); break;
		case WM_CLOSE: ProcessClose(); break;
		case WM_SIZE: FixPositions(); break;
		default: return ClassWindow::WndProc(hWnd, msg, wParam, lParam); break;
	}
	return 0;
}