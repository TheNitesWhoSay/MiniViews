#ifndef GENERALTAB_H
#define GENERALTAB_H
#include "../../WindowsLib/WindowsUi.h"

class GeneralWindow : public WinLib::ClassWindow
{
    public:
        bool CreateThis(HWND hParent, u64 windowId, int dpi, HFONT font);
        bool DestroyThis();
        void RefreshWindow();
        void FixPositions(int dpi, HFONT font);

    protected:
        void CreateSubWindows(int dpi, HFONT font);
        void EditModeCheckClicked();
        virtual void NotifyButtonClicked(int idFrom, HWND hWndFrom); // Sent when a button or checkbox is clicked
        virtual void NotifyEditUpdated(int idFrom, HWND hWndFrom); // Sent when edit text changes, before redraw

    private:
        WinLib::CheckBoxControl checkEditMode;
        WinLib::ButtonControl buttonAddMiniView;
        WinLib::ButtonControl buttonClearMiniViews;
        WinLib::EditControl editOpacity;

        static const std::string checkEditModeTooltip;
        static const std::string addMiniViewTooltip;
        static const std::string opacityTooltip;
};

#endif