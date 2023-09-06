#ifndef MAINDIALOG_H
#define MAINDIALOG_H
#include "../../WindowsLib/WindowsUi.h"
#include "GeneralTab.h"
#include "ViewsTab.h"
#include "AdvancedTab.h"
#include "AboutTab.h"

class MainDialog : public WinLib::ClassWindow
{
    public:

        MainDialog();
        ~MainDialog();
        GeneralWindow generalWindow;
        ViewsWindow viewsWindow;
        AdvancedWindow advancedWindow;
        AboutWindow aboutWindow;
        bool CreateThis(HWND hParent);
        bool DestroyThis();
        bool IsShown();
        void ToggleShown(HWND hParent);
        void ChangeTab(u32 tabId);
        void RefreshGeneralTab();
        void RefreshViewsTab(bool rebuildTree, bool refreshNames); // refreshNames is redundant/ignored when rebuildTree is set


    protected:

        void FixPositions(int dpi);
        void ProcessClose();
        void CreateSubWindows(int dpi);
        void TabSelChange();
        void TabSelChanging();
        void DpiChanged(int dpi, RECT* newRect);
        LRESULT SysCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
        LRESULT Notify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr);
        LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


    private:

        WinLib::TabControl tabs;

        int dpi;
        int windowLeft, windowTop;
        enum class TabId : u32 { GeneralTab = 0, ViewsTab, AdvancedTab, AboutTab };
        TabId selectedTab;

        HFONT defaultFont;
};

#endif