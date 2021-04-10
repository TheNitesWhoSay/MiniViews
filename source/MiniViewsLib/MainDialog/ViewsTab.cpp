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

bool ViewsWindow::CreateThis(HWND hParent, u64 windowId)
{
	if ( getHandle() != NULL )
		return SetParent(hParent);

	RECT rcCli = {};
	if ( GetClientRect(hParent, &rcCli) &&
		ClassWindow::RegisterWindowClass(NULL, NULL, NULL, NULL, NULL, "ViewsWindow", NULL, false) &&
		ClassWindow::CreateClassWindow(NULL, "Views Tab", WS_CHILD,
			5, 22, rcCli.right - rcCli.left - 5, rcCli.bottom - rcCli.top - 22, hParent, (HMENU)windowId) )
	{
		CreateSubWindows();
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

			std::string miniViewTitle = miniView->GetWinText();
			HTREEITEM treeItem = treeMiniViews.InsertTreeItem(NULL, miniView->GetWinText().c_str(), i);
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
			if ( std::get<1>(tuple).compare(std::get<2>(tuple)->GetWinText()) != 0 )
			{
				std::get<1>(tuple) = std::get<2>(tuple)->GetWinText();
				treeMiniViews.SetItemText(std::get<0>(tuple), std::get<2>(tuple)->GetWinText().c_str());
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

void ViewsWindow::FixPositions()
{
	treeMiniViews.SetPos(0, 5, 100, cliHeight() - 10);
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

void ViewsWindow::CreateSubWindows()
{
	treeMiniViews.CreateThis(getHandle(), 0, 5, 100, cliHeight() - 10, false, 0);

	groupSize.CreateThis(getHandle(), treeMiniViews.Right() + 5, 5, 145, 179, "Window Settings", 0);
	textWindowXc.CreateThis(getHandle(), groupSize.Left() + 10, groupSize.Top() + 20, 45, 23, "X-Pos: ", 0);
	textWindowYc.CreateThis(getHandle(), groupSize.Left() + 10, textWindowXc.Bottom() + 2, 45, 23, "Y-Pos: ", 0);
	textWindowWidth.CreateThis(getHandle(), groupSize.Left() + 10, textWindowYc.Bottom() + 2, 45, 23, "Width: ", 0);
	textWindowHeight.CreateThis(getHandle(), groupSize.Left() + 10, textWindowWidth.Bottom() + 2, 45, 23, "Height: ", 0);
	editWindowXc.CreateThis(getHandle(), textWindowXc.Right() + 10, textWindowXc.Top() - 3, 50, 23, false, (u32)Id::EditWindowXPos);
	editWindowYc.CreateThis(getHandle(), textWindowYc.Right() + 10, textWindowYc.Top() - 3, 50, 23, false, (u32)Id::EditWindowYPos);
	editWindowWidth.CreateThis(getHandle(), textWindowWidth.Right() + 10, textWindowWidth.Top() - 3,
		50, 23, false, (u32)Id::EditWindowWidth);
	editWindowHeight.CreateThis(getHandle(), textWindowHeight.Right() + 10, textWindowHeight.Top() - 3,
		50, 23, false, (u32)Id::EditWindowHeight);
	buttonMatchSourcePos.CreateThis(getHandle(), groupSize.Left() + 10, editWindowHeight.Bottom() + 5,
		125, 23, "Match Source Position", (u32)Id::ButtonMatchSourcePos);
	buttonMatchSourceSize.CreateThis(getHandle(), groupSize.Left() + 10, buttonMatchSourcePos.Bottom() + 5,
		125, 23, "Match Source Size", (u32)Id::ButtonMatchSourceSize);

	groupClip.CreateThis(getHandle(), treeMiniViews.Right() + 5, groupSize.Bottom() + 5, 145, 155, "Clip Settings", 0);
	checkClipped.CreateThis(getHandle(), groupClip.Left() + 10, groupClip.Top() + 20, 55, 23, false, "Clipped", (u32)Id::CheckClipped);
	textClipLeft.CreateThis(getHandle(), groupClip.Left() + 10, checkClipped.Bottom() + 10, 40, 23, "Left: ", 0);
	textClipTop.CreateThis(getHandle(), groupClip.Left() + 10, textClipLeft.Bottom() + 2, 40, 23, "Top: ", 0);
	textClipRight.CreateThis(getHandle(), groupClip.Left() + 10, textClipTop.Bottom() + 2, 40, 23, "Right: ", 0);
	textClipBottom.CreateThis(getHandle(), groupClip.Left() + 10, textClipRight.Bottom() + 2, 40, 23, "Bottom: ", 0);
	editClipLeft.CreateThis(getHandle(), textClipLeft.Right() + 10, textClipLeft.Top() - 3, 50, 23, false, (u32)Id::EditClipLeft);
	editClipTop.CreateThis(getHandle(), textClipTop.Right() + 10, textClipTop.Top() - 3, 50, 23, false, (u32)Id::EditClipTop);
	editClipRight.CreateThis(getHandle(), textClipRight.Right() + 10, textClipRight.Top() - 3, 50, 23, false, (u32)Id::EditClipRight);
	editClipBottom.CreateThis(getHandle(), textClipBottom.Right() + 10, textClipBottom.Top() - 3, 50, 23, false, (u32)Id::EditClipBottom);
	buttonSetClip.CreateThis(getHandle(), editClipLeft.Right() - 30, groupClip.Top() + 20, 30, 23, "Set", (u32)Id::ButtonSetClipArea);

	checkLockSizeRatio.CreateThis(getHandle(), treeMiniViews.Right() + 5, groupClip.Bottom() + 5,
		150, 23, false, "Lock Size Ratio", (u32)Id::CheckLockSizeRatio);
	checkHideWhenSourceOnTop.CreateThis(getHandle(), treeMiniViews.Right() + 5, checkLockSizeRatio.Bottom() + 2,
		150, 23, false, "Hide When Source On Top", (u32)Id::CheckHideWhenSourceOnTop);
}

void ViewsWindow::NotifyTreeSelChanged(LPARAM newValue)
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
