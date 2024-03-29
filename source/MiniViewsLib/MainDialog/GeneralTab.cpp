#include "GeneralTab.h"
#include "../MiniViews.h"

enum class Id
{
    CheckEditMode = ID_FIRST,
    ButtonAddMiniView,
    ButtonAddMiniViewTip,
    ButtonClearMiniView,
    EditOpacity,
    EditOpacityBuddy
};

const std::string GeneralWindow::checkEditModeTooltip("(Ctrl Shift E)");
const std::string GeneralWindow::addMiniViewTooltip("(Ctrl Shift A)");
const std::string GeneralWindow::opacityTooltip("(Ctrl Shift -)/(Ctrl Shift +)");

bool GeneralWindow::CreateThis(HWND hParent, u64 windowId, int dpi, HFONT font)
{
    if ( getHandle() != NULL )
        return SetParent(hParent);

    int xPadding = DpiScale(5, dpi);
    int yPadding = DpiScale(22, dpi);
    RECT rcCli = {};
    if ( GetClientRect(hParent, &rcCli) &&
        ClassWindow::RegisterWindowClass(NULL, NULL, NULL, NULL, NULL, "GeneralWindow", NULL, false) &&
        ClassWindow::CreateClassWindow(NULL, "General Tab", WS_CHILD,
            xPadding, yPadding, rcCli.right - rcCli.left-xPadding, rcCli.bottom - rcCli.top-yPadding, hParent, (HMENU)windowId) )
    {
        CreateSubWindows(dpi, font);
        return true;
    }
    else
        return false;
}

bool GeneralWindow::DestroyThis()
{
    ClassWindow::DestroyThis();
    return true;
}

void GeneralWindow::RefreshWindow()
{
    if ( getHandle() != NULL )
    {
        checkEditMode.SetCheck(miniViews.IsInEditMode());
        u8 displayedOpacityValue = 0;
        if ( !editOpacity.GetEditNum<u8>(displayedOpacityValue) || displayedOpacityValue != miniViews.GetOpacityLevel() )
            editOpacity.SetEditNum(miniViews.GetOpacityLevel());
    }
}

void GeneralWindow::FixPositions(int dpi, HFONT font)
{

}

void GeneralWindow::CreateSubWindows(int dpi, HFONT font)
{
    WindowsItem::setFont(font, false);
    int padding = DpiScale(5, dpi);
    bool editMode = miniViews.IsInEditMode();
    checkEditMode.CreateThis(getHandle(), 0, DpiScale(5, dpi), DpiScale(100, dpi), DpiScale(23, dpi), editMode, "Edit Mode", (u32)Id::CheckEditMode);
    checkEditMode.AddTooltip(checkEditModeTooltip.c_str());
    buttonAddMiniView.CreateThis(getHandle(), 0, checkEditMode.Bottom() + padding, DpiScale(100, dpi), DpiScale(23, dpi), "Add Mini View", (u32)Id::ButtonAddMiniView);
    buttonAddMiniView.AddTooltip(addMiniViewTooltip.c_str());
    buttonClearMiniViews.CreateThis(getHandle(), 0, buttonAddMiniView.Bottom() + padding, DpiScale(100, dpi), DpiScale(23, dpi), "Clear Mini Views", (u32)Id::ButtonClearMiniView);
    WinLib::TextControl textOpacity;
    textOpacity.CreateThis(getHandle(), 0, buttonClearMiniViews.Bottom() + padding+DpiScale(3, dpi), DpiScale(111, dpi), DpiScale(23, dpi), "Opacity Level (0-255): ", 0);
    editOpacity.CreateThis(getHandle(), textOpacity.Right()+padding, textOpacity.Top()-DpiScale(3, dpi), DpiScale(75, dpi), DpiScale(23, dpi), false, (u32)Id::EditOpacity);
    editOpacity.AddTooltip(opacityTooltip.c_str());
    editOpacity.SetTextLimit(3);
    editOpacity.SetEditNum(miniViews.GetOpacityLevel());
    HWND hTransparencyBuddy = CreateWindowEx(NULL, UPDOWN_CLASS, NULL, WS_CHILDWINDOW | WS_VISIBLE | WS_DISABLED | UDS_SETBUDDYINT |
        UDS_ALIGNRIGHT | UDS_ARROWKEYS | UDS_HOTTRACK, 0, 0, 0, 0, getHandle(), (HMENU)Id::EditOpacityBuddy, NULL, NULL);
    SendMessage(hTransparencyBuddy, UDM_SETBUDDY, (WPARAM)editOpacity.getHandle(), NULL);
    SendMessage(hTransparencyBuddy, UDM_SETRANGE32, 0, MAKELPARAM(255, 0));
    EnableWindow(hTransparencyBuddy, TRUE);
}

void GeneralWindow::EditModeCheckClicked()
{
    if ( checkEditMode.isChecked() != miniViews.IsInEditMode() )
        miniViews.ToggleEditMode();

    checkEditMode.SetCheck(miniViews.IsInEditMode());
}

void GeneralWindow::NotifyButtonClicked(int idFrom, HWND)
{
    switch ( idFrom )
    {
        case (int)Id::CheckEditMode: EditModeCheckClicked(); break;
        case (int)Id::ButtonAddMiniView: miniViews.AddMiniView(); break;
        case (int)Id::ButtonClearMiniView: miniViews.ClearMiniViews(); break;
    }
}

void GeneralWindow::NotifyEditUpdated(int idFrom, HWND)
{
    if ( idFrom == (int)Id::EditOpacity )
    {
        int newOpacity = 0;
        if ( editOpacity.GetEditNum<int>(newOpacity) )
        {
            if ( newOpacity > 255 )
                newOpacity = 255;
            else if ( newOpacity < 0 )
                newOpacity = 0;
            
            miniViews.SetOpacityLevel((u8)newOpacity);
        }
        else if ( editOpacity.GetTextLength() != 0 )
            editOpacity.SetEditNum(miniViews.GetOpacityLevel());
    }
}
