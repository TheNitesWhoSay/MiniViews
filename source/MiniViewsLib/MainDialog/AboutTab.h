#ifndef ABOUTTAB_H
#define ABOUTTAB_H
#include "../../WindowsLib/WindowsUi.h"
#include "../Common.h"

class AboutWindow : public WinLib::ClassWindow
{
    public:
        bool CreateThis(HWND hParent, u64 windowId, int dpi, HFONT font);
        bool DestroyThis();
        void RefreshWindow();
        void FixPositions(int dpi, HFONT font);

    protected:
        void CreateSubWindows(int dpi, HFONT font);
        void NotifyButtonClicked(int idFrom, HWND hWndFrom) override; // Sent when a button or checkbox is clicked

    private:
        WinLib::ButtonControl buttonWebsite;
        WinLib::ButtonControl buttonManual;
};

#endif