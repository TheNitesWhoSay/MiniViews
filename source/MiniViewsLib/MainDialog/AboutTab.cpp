#include "AboutTab.h"
#include "../Preferences.h"
#include "../MiniViews.h"
#include "../Version.h"

enum class Id : int
{
    ButtonWebsite,
    ButtonManual
};

bool AboutWindow::CreateThis(HWND hParent, u64 windowId, int dpi, HFONT font)
{
    if ( getHandle() != NULL )
        return SetParent(hParent);

    RECT rcCli = {};
    if ( GetClientRect(hParent, &rcCli) &&
        ClassWindow::RegisterWindowClass(NULL, NULL, NULL, NULL, NULL, "AboutWindow", NULL, false) &&
        ClassWindow::CreateClassWindow(NULL, "About Tab", WS_CHILD,
            DpiScale(5, dpi), DpiScale(22, dpi), rcCli.right - rcCli.left - DpiScale(5, dpi), rcCli.bottom - rcCli.top - DpiScale(22, dpi), hParent, (HMENU)windowId) )
    {
        CreateSubWindows(dpi, font);
        return true;
    }
    else
        return false;
}

bool AboutWindow::DestroyThis()
{
    ClassWindow::DestroyThis();
    return true;
}

void AboutWindow::RefreshWindow()
{

}

void AboutWindow::FixPositions(int dpi, HFONT font)
{

}

void AboutWindow::CreateSubWindows(int dpi, HFONT font)
{
    WindowsItem::SetFont(font, false);
    int padding = DpiScale(5, dpi);
    buttonWebsite.CreateThis(getHandle(), 0, DpiScale(5, dpi), DpiScale(100, dpi), DpiScale(23, dpi), "Website", (u32)Id::ButtonWebsite);
    buttonManual.CreateThis(getHandle(), 0, buttonWebsite.Bottom() + padding, DpiScale(100, dpi), DpiScale(23, dpi), "Manual", (u32)Id::ButtonManual);

    WinLib::TextControl textCopyright, textVersion;
    textCopyright.CreateThis(getHandle(), DpiScale(5, dpi), cliHeight() - DpiScale(30, dpi), cliWidth() - DpiScale(10, dpi), DpiScale(20, dpi),
        "All rights reserved (c) 2016 Justin Forsberg.", 0);
    textVersion.CreateThis(getHandle(), DpiScale(5, dpi), textCopyright.Top() - DpiScale(25, dpi), cliWidth() - DpiScale(10, dpi), DpiScale(20, dpi),
        std::string("Full Version ID: " + GetFullVersionString()).c_str(), 0);
}

void AboutWindow::NotifyButtonClicked(int idFrom, HWND)
{
    switch ( (Id)idFrom )
    {
        case Id::ButtonWebsite: ShellExecute(NULL, icux::toUistring("open").c_str(), icux::toUistring("https://github.com/TheNitesWhoSay/MiniViews").c_str(), NULL, NULL, SW_SHOWNORMAL); break;
        case Id::ButtonManual: ShellExecute(NULL, icux::toUistring("open").c_str(), icux::toUistring("https://github.com/TheNitesWhoSay/MiniViews/blob/master/README.md").c_str(), NULL, NULL, SW_SHOWNORMAL); break;
    }
}
