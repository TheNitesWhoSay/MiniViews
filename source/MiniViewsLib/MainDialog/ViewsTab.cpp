#include "ViewsTab.h"
#include "../MiniViews.h"

enum class Id : int
{
    EditWindowXPos = ID_FIRST,
    EditWindowYPos,
    EditWindowWidth,
    EditWindowHeight,
    ButtonMatchSourcePos,
    ButtonMatchSourceSize,
    CheckClipped,
    ButtonSetClipArea,
    EditClipLeft,
    EditClipTop,
    EditClipRight,
    EditClipBottom,
    CheckLockSizeRatio,
    CheckHideWhenSourceOnTop
};

bool ViewsWindow::CreateThis(HWND hParent, u64 windowId, int dpi, HFONT font)
{
    if ( getHandle() != NULL )
        return SetParent(hParent);

    RECT rcCli = {};
    if ( GetClientRect(hParent, &rcCli) &&
        ClassWindow::RegisterWindowClass(NULL, NULL, NULL, NULL, NULL, "ViewsWindow", NULL, false) &&
        ClassWindow::CreateClassWindow(NULL, "Views Tab", WS_CHILD,
            DpiScale(5, dpi), DpiScale(22, dpi), rcCli.right - rcCli.left - DpiScale(5, dpi), rcCli.bottom - rcCli.top - DpiScale(22, dpi), hParent, (HMENU)windowId) )
    {
        CreateSubWindows(dpi, font);
        return true;
    }
    else
        return false;
}

bool ViewsWindow::DestroyThis()
{
    ClassWindow::DestroyThis();
    return true;
}

void ViewsWindow::RefreshWindow(bool rebuildTree, bool refreshNames)
{
    if ( rebuildTree )
    {
        LPARAM i = 0;
        currTreeItems.clear();
        auto miniViewList = miniViews.GetMiniViewList();
        bool includesCurrMiniView = false;
        treeMiniViews.SetRedraw(false);
        treeMiniViews.EmptySubTree(NULL);
        for ( auto &miniView : miniViewList )
        {
            if ( currMiniView == miniView )
                includesCurrMiniView = true;

            std::string miniViewTitle = miniView->GetWinText().value();
            HTREEITEM treeItem = treeMiniViews.InsertTreeItem(NULL, miniView->GetWinText().value(), i);
            currTreeItems.push_back(std::tuple<HTREEITEM, std::string, std::shared_ptr<MiniView>>(treeItem, miniViewTitle, miniView));
            i++;
        }
        treeMiniViews.SetRedraw(true);

        if ( !includesCurrMiniView )
            currMiniView = (miniViewList.size() > 0 ? miniViewList[0] : nullptr);
    }
    else if ( refreshNames ) // Ensure titles are correct
    {
        for ( auto &tuple : currTreeItems )
        {
            if ( std::get<1>(tuple).compare(std::get<2>(tuple)->GetWinText().value()) != 0 )
            {
                std::get<1>(tuple) = std::get<2>(tuple)->GetWinText().value();
                treeMiniViews.SetItemText(std::get<0>(tuple), std::get<2>(tuple)->GetWinText().value());
            }
        }
    }

    if ( currMiniView != nullptr )
    {
        EnableEditing();
        editWindowXc.SetEditNum<int>(currMiniView->Left());
        editWindowYc.SetEditNum<int>(currMiniView->Top());
        editWindowWidth.SetEditNum<int>(currMiniView->cliWidth());
        editWindowHeight.SetEditNum<int>(currMiniView->cliHeight());

        checkClipped.SetCheck(currMiniView->IsClipped());
        RECT clipRect = currMiniView->GetClipRect();
        editClipLeft.SetEditNum<int>(clipRect.left);
        editClipTop.SetEditNum<int>(clipRect.top);
        editClipRight.SetEditNum<int>(clipRect.right);
        editClipBottom.SetEditNum<int>(clipRect.bottom);

        if ( currMiniView->IsClipped() )
        {
            editClipLeft.EnableThis();
            editClipTop.EnableThis();
            editClipRight.EnableThis();
            editClipBottom.EnableThis();
        }
        else
        {
            editClipLeft.DisableThis();
            editClipTop.DisableThis();
            editClipRight.DisableThis();
            editClipBottom.DisableThis();
        }

        checkHideWhenSourceOnTop.SetCheck(currMiniView->HidesWhenSourceOnTop());
        checkLockSizeRatio.SetCheck(currMiniView->SizeRatioLocked());
    }
    else
        DisableEditing();
}

void ViewsWindow::FixPositions(int dpi, HFONT font)
{
    treeMiniViews.SetPos(0, DpiScale(5, dpi), DpiScale(100, dpi), cliHeight() - DpiScale(10, dpi));
}

void ViewsWindow::EnableEditing()
{
    groupSize.EnableThis();
    textWindowXc.EnableThis();
    textWindowYc.EnableThis();
    textWindowWidth.EnableThis();
    textWindowHeight.EnableThis();
    editWindowXc.EnableThis();
    editWindowYc.EnableThis();
    editWindowWidth.EnableThis();
    editWindowHeight.EnableThis();
    buttonMatchSourcePos.EnableThis();
    buttonMatchSourceSize.EnableThis();
    groupClip.EnableThis();
    checkClipped.EnableThis();
    buttonSetClip.EnableThis();
    textClipLeft.EnableThis();
    textClipTop.EnableThis();
    textClipRight.EnableThis();
    textClipBottom.EnableThis();
    editClipLeft.EnableThis();
    editClipTop.EnableThis();
    editClipRight.EnableThis();
    editClipBottom.EnableThis();
    checkLockSizeRatio.EnableThis();
    checkHideWhenSourceOnTop.EnableThis();
}

void ViewsWindow::DisableEditing()
{
    groupSize.DisableThis();
    textWindowXc.DisableThis();
    textWindowYc.DisableThis();
    textWindowWidth.DisableThis();
    textWindowHeight.DisableThis();
    editWindowXc.DisableThis();
    editWindowYc.DisableThis();
    editWindowWidth.DisableThis();
    editWindowHeight.DisableThis();
    buttonMatchSourcePos.DisableThis();
    buttonMatchSourceSize.DisableThis();
    groupClip.DisableThis();
    checkClipped.DisableThis();
    buttonSetClip.DisableThis();
    textClipLeft.DisableThis();
    textClipTop.DisableThis();
    textClipRight.DisableThis();
    textClipBottom.DisableThis();
    editClipLeft.DisableThis();
    editClipTop.DisableThis();
    editClipRight.DisableThis();
    editClipBottom.DisableThis();
    checkLockSizeRatio.DisableThis();
    checkHideWhenSourceOnTop.DisableThis();
    editWindowXc.SetText("");
    editWindowYc.SetText("");
    editWindowWidth.SetText("");
    editWindowHeight.SetText("");
    checkClipped.SetCheck(false);
    editClipLeft.SetText("");
    editClipTop.SetText("");
    editClipRight.SetText("");
    editClipBottom.SetText("");
    checkLockSizeRatio.SetCheck(false);
    checkHideWhenSourceOnTop.SetCheck(false);
}

void ViewsWindow::CreateSubWindows(int dpi, HFONT font)
{
    WindowsItem::setFont(font, false);
    treeMiniViews.CreateThis(getHandle(), 0, DpiScale(5, dpi), DpiScale(100, dpi), cliHeight() - DpiScale(10, dpi), false, 0);

    groupSize.CreateThis(getHandle(), treeMiniViews.Right() + DpiScale(5, dpi), DpiScale(5, dpi), DpiScale(145, dpi), DpiScale(179, dpi), "Window Settings", 0);
    textWindowXc.CreateThis(getHandle(), groupSize.Left() + DpiScale(10, dpi), groupSize.Top() + DpiScale(20, dpi), DpiScale(45, dpi), DpiScale(23, dpi), "X-Pos: ", 0);
    textWindowYc.CreateThis(getHandle(), groupSize.Left() + DpiScale(10, dpi), textWindowXc.Bottom() + DpiScale(2, dpi), DpiScale(45, dpi), DpiScale(23, dpi), "Y-Pos: ", 0);
    textWindowWidth.CreateThis(getHandle(), groupSize.Left() + DpiScale(10, dpi), textWindowYc.Bottom() + DpiScale(2, dpi), DpiScale(45, dpi), DpiScale(23, dpi), "Width: ", 0);
    textWindowHeight.CreateThis(getHandle(), groupSize.Left() + DpiScale(10, dpi), textWindowWidth.Bottom() + DpiScale(2, dpi), DpiScale(45, dpi), DpiScale(23, dpi), "Height: ", 0);
    editWindowXc.CreateThis(getHandle(), textWindowXc.Right() + DpiScale(10, dpi), textWindowXc.Top() - DpiScale(3, dpi), DpiScale(50, dpi), DpiScale(23, dpi), false, (u32)Id::EditWindowXPos);
    editWindowYc.CreateThis(getHandle(), textWindowYc.Right() + DpiScale(10, dpi), textWindowYc.Top() - DpiScale(3, dpi), DpiScale(50, dpi), DpiScale(23, dpi), false, (u32)Id::EditWindowYPos);
    editWindowWidth.CreateThis(getHandle(), textWindowWidth.Right() + DpiScale(10, dpi), textWindowWidth.Top() - DpiScale(3, dpi),
        DpiScale(50, dpi), DpiScale(23, dpi), false, (u32)Id::EditWindowWidth);
    editWindowHeight.CreateThis(getHandle(), textWindowHeight.Right() + DpiScale(10, dpi), textWindowHeight.Top() - DpiScale(3, dpi),
        DpiScale(50, dpi), DpiScale(23, dpi), false, (u32)Id::EditWindowHeight);
    buttonMatchSourcePos.CreateThis(getHandle(), groupSize.Left() + DpiScale(10, dpi), editWindowHeight.Bottom() + DpiScale(5, dpi),
        DpiScale(125, dpi), DpiScale(23, dpi), "Match Source Position", (u32)Id::ButtonMatchSourcePos);
    buttonMatchSourceSize.CreateThis(getHandle(), groupSize.Left() + DpiScale(10, dpi), buttonMatchSourcePos.Bottom() + DpiScale(5, dpi),
        DpiScale(125, dpi), DpiScale(23, dpi), "Match Source Size", (u32)Id::ButtonMatchSourceSize);

    groupClip.CreateThis(getHandle(), treeMiniViews.Right() + DpiScale(5, dpi), groupSize.Bottom() + DpiScale(5, dpi), DpiScale(145, dpi), DpiScale(155, dpi), "Clip Settings", 0);
    checkClipped.CreateThis(getHandle(), groupClip.Left() + DpiScale(10, dpi), groupClip.Top() + DpiScale(20, dpi), DpiScale(55, dpi), DpiScale(23, dpi), false, "Clipped", (u32)Id::CheckClipped);
    textClipLeft.CreateThis(getHandle(), groupClip.Left() + DpiScale(10, dpi), checkClipped.Bottom() + DpiScale(10, dpi), DpiScale(40, dpi), DpiScale(23, dpi), "Left: ", 0);
    textClipTop.CreateThis(getHandle(), groupClip.Left() + DpiScale(10, dpi), textClipLeft.Bottom() + DpiScale(2, dpi), DpiScale(40, dpi), DpiScale(23, dpi), "Top: ", 0);
    textClipRight.CreateThis(getHandle(), groupClip.Left() + DpiScale(10, dpi), textClipTop.Bottom() + DpiScale(2, dpi), DpiScale(40, dpi), DpiScale(23, dpi), "Right: ", 0);
    textClipBottom.CreateThis(getHandle(), groupClip.Left() + DpiScale(10, dpi), textClipRight.Bottom() + DpiScale(2, dpi), DpiScale(40, dpi), DpiScale(23, dpi), "Bottom: ", 0);
    editClipLeft.CreateThis(getHandle(), textClipLeft.Right() + DpiScale(10, dpi), textClipLeft.Top() - DpiScale(3, dpi), DpiScale(50, dpi), DpiScale(23, dpi), false, (u32)Id::EditClipLeft);
    editClipTop.CreateThis(getHandle(), textClipTop.Right() + DpiScale(10, dpi), textClipTop.Top() - DpiScale(3, dpi), DpiScale(50, dpi), DpiScale(23, dpi), false, (u32)Id::EditClipTop);
    editClipRight.CreateThis(getHandle(), textClipRight.Right() + DpiScale(10, dpi), textClipRight.Top() - DpiScale(3, dpi), DpiScale(50, dpi), DpiScale(23, dpi), false, (u32)Id::EditClipRight);
    editClipBottom.CreateThis(getHandle(), textClipBottom.Right() + DpiScale(10, dpi), textClipBottom.Top() - DpiScale(3, dpi), DpiScale(50, dpi), DpiScale(23, dpi), false, (u32)Id::EditClipBottom);
    buttonSetClip.CreateThis(getHandle(), editClipLeft.Right() - DpiScale(30, dpi), groupClip.Top() + DpiScale(20, dpi), DpiScale(30, dpi), DpiScale(23, dpi), "Set", (u32)Id::ButtonSetClipArea);

    checkLockSizeRatio.CreateThis(getHandle(), treeMiniViews.Right() + DpiScale(5, dpi), groupClip.Bottom() + DpiScale(5, dpi),
        DpiScale(150, dpi), DpiScale(23, dpi), false, "Lock Size Ratio", (u32)Id::CheckLockSizeRatio);
    checkHideWhenSourceOnTop.CreateThis(getHandle(), treeMiniViews.Right() + DpiScale(5, dpi), checkLockSizeRatio.Bottom() + DpiScale(2, dpi),
        DpiScale(150, dpi), DpiScale(23, dpi), false, "Hide When Source On Top", (u32)Id::CheckHideWhenSourceOnTop);
}

void ViewsWindow::NotifyTreeItemSelected(LPARAM newValue)
{
    currMiniView = nullptr;
    LPARAM i = 0;
    auto miniViewList = miniViews.GetMiniViewList();
    for ( auto &miniView : miniViewList )
    {
        if ( i == newValue )
        {
            currMiniView = miniView;
            break;
        }
        i++;
    }
    RefreshWindow(false, false);
}

void ViewsWindow::NotifyButtonClicked(int idFrom, HWND)
{
    if ( currMiniView != nullptr )
    {
        bool performedCommand = true;
        switch ( (Id)idFrom )
        {
            case Id::ButtonMatchSourcePos: currMiniView->MatchSource(true, false); break;
            case Id::ButtonMatchSourceSize: currMiniView->MatchSource(false, true); break;
            case Id::ButtonSetClipArea: currMiniView->SetClipRegion(); break;
            case Id::CheckClipped: currMiniView->ClearClipRegion(); break;
            case Id::CheckLockSizeRatio: currMiniView->ToggleLockRatio(); break;
            case Id::CheckHideWhenSourceOnTop: currMiniView->ToggleHideWhenSourceOnTop(); break;
            default: performedCommand = false; break;
        }

        if ( performedCommand )
            RefreshWindow(false, false);
    }
}

void ViewsWindow::NotifyEditFocusLost(int idFrom, HWND)
{
    if ( currMiniView != nullptr )
    {
        int num = 0;
        bool performedCommand = true;
        switch ( (Id)idFrom )
        {
            case Id::EditWindowXPos: if ( editWindowXc.GetEditNum(num) ) currMiniView->AdjustWindowLeft(num); break;
            case Id::EditWindowYPos: if ( editWindowYc.GetEditNum(num) ) currMiniView->AdjustWindowTop(num); break;
            case Id::EditWindowWidth:
                if ( editWindowWidth.GetEditNum(num) /*&& num != currMiniView->cliWidth()*/ )
                    currMiniView->AdjustCliWidth(num);
                break;
            case Id::EditWindowHeight:
                if ( editWindowHeight.GetEditNum(num) /*&& num != currMiniView->cliHeight()*/ )
                    currMiniView->AdjustCliHeight(num);
                break;
            case Id::EditClipLeft: if ( editClipLeft.GetEditNum(num) ) currMiniView->AdjustClipLeft(num); break;
            case Id::EditClipTop: if ( editClipTop.GetEditNum(num) ) currMiniView->AdjustClipTop(num); break;
            case Id::EditClipRight: if ( editClipRight.GetEditNum(num) ) currMiniView->AdjustClipRight(num); break;
            case Id::EditClipBottom: if ( editClipBottom.GetEditNum(num) ) currMiniView->AdjustClipBottom(num); break;
            default: performedCommand = false; break;
        }

        if ( performedCommand )
            RefreshWindow(false, false);
    }
}
