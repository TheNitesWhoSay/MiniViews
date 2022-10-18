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

MainDialog::MainDialog() : dpi(DefaultDpi), windowLeft(0), windowTop(0), selectedTab(TabId::GeneralTab), defaultFont(NULL), smallIcon(NULL),
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
    this->dpi = hParent != NULL ? GetDpiForWindow(hParent) : GetDpiForSystem();
    defaultFont = ::CreateFont(DpiScale(14, dpi), DpiScale(5, dpi), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, icux::toUistring("Microsoft Sans Serif").c_str());
    if ( ClassWindow::RegisterWindowClass(NULL, mediumIcon, NULL, GetSysColorBrush(COLOR_WINDOW), NULL, "MainMiniViewsDialog", smallIcon, false) &&
         ClassWindow::CreateClassWindow(WS_EX_APPWINDOW, MiniViews::mainWindowName.c_str(), WS_SYSMENU|WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT,
             DpiScale(279, dpi), DpiScale(464, dpi), hParent, NULL) )
    {
        CreateSubWindows(dpi);
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

void MainDialog::FixPositions(int dpi)
{
    RECT rcCli = {};
    getClientRect(rcCli);
    tabs.SetPos(rcCli.left, rcCli.top, rcCli.right - rcCli.left, rcCli.bottom - rcCli.top);
    tabs.getClientRect(rcCli);
    
    int xPadding = DpiScale(5, dpi);
    int yPadding = DpiScale(22, dpi);
    generalWindow.SetPos(rcCli.left+xPadding, rcCli.top+yPadding, rcCli.right - rcCli.left-xPadding, rcCli.bottom - rcCli.top-yPadding);
    viewsWindow.SetPos(rcCli.left+xPadding, rcCli.top+yPadding, rcCli.right - rcCli.left-xPadding, rcCli.bottom - rcCli.top-yPadding);
    advancedWindow.SetPos(rcCli.left+xPadding, rcCli.top+yPadding, rcCli.right - rcCli.left-xPadding, rcCli.bottom - rcCli.top-yPadding);
    aboutWindow.SetPos(rcCli.left+xPadding, rcCli.top+yPadding, rcCli.right - rcCli.left-xPadding, rcCli.bottom - rcCli.top-yPadding);
    generalWindow.FixPositions(dpi, defaultFont);
    viewsWindow.FixPositions(dpi, defaultFont);
    advancedWindow.FixPositions(dpi, defaultFont);
    aboutWindow.FixPositions(dpi, defaultFont);
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

void MainDialog::CreateSubWindows(int dpi)
{
    WindowsItem::SetSmallIcon(smallIcon);
    WindowsItem::SetMedIcon(mediumIcon);
    //MoveTo(windowLeft, windowTop);

    tabs.CreateThis(getHandle(), 0, 0, DpiScale(263, dpi), DpiScale(426, dpi), (u32)Id::MainDialogTabs);
    //tabs.FindThis(getHandle(), IDC_MAINDIALOGTABS);
    std::vector<std::string> tabLabels = { "General", "Views", "Advanced", "About" };
    for ( size_t i = 0; i < tabLabels.size(); i++ )
        tabs.InsertTab((u32)i, tabLabels[i]);

    generalWindow.CreateThis(tabs.getHandle(), (u32)Id::GeneralWindow, dpi, defaultFont);
    viewsWindow.CreateThis(tabs.getHandle(), (u32)Id::ViewsWindow, dpi, defaultFont);
    advancedWindow.CreateThis(tabs.getHandle(), (u32)Id::AdvancedWindow, dpi, defaultFont);
    aboutWindow.CreateThis(tabs.getHandle(), (u32)Id::AboutWindow, dpi, defaultFont);

    ChangeTab((u32)selectedTab);
    switch ( selectedTab )
    {
        case TabId::GeneralTab: generalWindow.Show(); break;
        case TabId::ViewsTab: viewsWindow.Show(); break;
        case TabId::AdvancedTab: advancedWindow.Show(); break;
        case TabId::AboutTab: aboutWindow.Show(); break;
    }
    WindowsItem::ReplaceChildFonts(defaultFont);
    FixPositions(dpi);
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

void MainDialog::DpiChanged(int dpi, RECT* newRect)
{
    // TODO: Need to dynamically resize on DPI change
    //HWND hWnd = getHandle();
    //SetWindowPos(hWnd, hWnd, newRect->left, newRect->top, newRect->right-newRect->left, newRect->bottom-newRect->top, SWP_NOZORDER | SWP_NOACTIVATE);
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
        case WM_DPICHANGED: DpiChanged(int(LOWORD(wParam)), (RECT*)lParam); break;
        case WM_SYSCOMMAND: return SysCommand(hWnd, wParam, lParam); break;
        case WM_CLOSE: ProcessClose(); break;
        case WM_SIZE: FixPositions(dpi); break;
        default: return ClassWindow::WndProc(hWnd, msg, wParam, lParam); break;
    }
    return 0;
}