#ifndef ABOUTTAB_H
#define ABOUTTAB_H
#include "../../WindowsLib/WindowsUi.h"

class AboutWindow : public WinLib::ClassWindow
{
    public:
        bool CreateThis(HWND hParent, u32 windowId);
        bool DestroyThis();
        void RefreshWindow();
        void FixPositions();

    protected:
        void CreateSubWindows();
        virtual void NotifyButtonClicked(int idFrom, HWND hWndFrom); // Sent when a button or checkbox is clicked

    private:
        WinLib::ButtonControl buttonWebsite;
        WinLib::ButtonControl buttonManual;
};

#endif