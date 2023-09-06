#ifndef CLIPSEL_H
#define CLIPSEL_H
#include "../WindowsLib/WindowsUi.h"
#include "MiniView.h"

class ClipSel : public WinLib::ClassWindow
{
    public:
        ClipSel();
        ~ClipSel();
        bool CreateThis(HWND hParent, MiniView &miniViewToClip);
        void DestroyThis();

    protected:
        void LButtonDown(int xc, int yc);
        void MouseMove(WPARAM wParam, int xc, int yc);
        void LButtonUp(int xc, int yc);
        void Paint();
        LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        LONG startX, startY;
        MiniView* miniView;
        RECT rcClip;
};

#endif