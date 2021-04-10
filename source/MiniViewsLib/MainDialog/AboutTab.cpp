#include "AboutTab.h"
#include "../Preferences.h"
#include "../MiniViews.h"
#include "../Version.h"

enum class Id : int
{
    ButtonWebsite,
    ButtonManual
};

bool AboutWindow::CreateThis(HWND hParent, u64 windowId)
{
    if ( getHandle() != NULL )
        return SetParent(hParent);

    RECT rcCli = {};
    if ( GetClientRect(hParent, &rcCli) &&
        ClassWindow::RegisterWindowClass(NULL, NULL, NULL, NULL, NULL, "AboutWindow", NULL, false) &&
        ClassWindow::CreateClassWindow(NULL, "About Tab", WS_CHILD,
            5, 22, rcCli.right - rcCli.left - 5, rcCli.bottom - rcCli.top - 22, hParent, (HMENU)windowId) )
    {
        CreateSubWindows();
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

void AboutWindow::FixPositions()
{

}

void AboutWindow::CreateSubWindows()
{
    int padding = 5;
    buttonWebsite.CreateThis(getHandle(), 0, 5, 100, 23, "Website", (u32)Id::ButtonWebsite);
    buttonManual.CreateThis(getHandle(), 0, buttonWebsite.Bottom() + padding, 100, 23, "Manual", (u32)Id::ButtonManual);

    WinLib::TextControl textCopyright, textVersion;
    textCopyright.CreateThis(getHandle(), 5, cliHeight() - 30, cliWidth() - 10, 20,
        "All rights reserved (c) 2016 Justin Forsberg.", 0);
    textVersion.CreateThis(getHandle(), 5, textCopyright.Top() - 25, cliWidth() - 10, 20,
        std::string("Full Version ID: " + GetFullVersionString()).c_str(), 0);
}

void AboutWindow::NotifyButtonClicked(int idFrom, HWND)
{
    switch ( (Id)idFrom )
    {
        case Id::ButtonWebsite: ShellExecute(NULL, "open", "https://github.com/TheNitesWhoSay/MiniViews", NULL, NULL, SW_SHOWNORMAL); break;
        case Id::ButtonManual: ShellExecute(NULL, "open", "https://github.com/TheNitesWhoSay/MiniViews/blob/master/README.md", NULL, NULL, SW_SHOWNORMAL); break;
    }
}
