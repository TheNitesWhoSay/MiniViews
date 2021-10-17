#ifndef VIEWSTAB_H
#define VIEWSTAB_H
#include "../../WindowsLib/WindowsUi.h"
#include "../MiniView.h"
#include <vector>
#include <memory>
#include <tuple>

class ViewsWindow : public WinLib::ClassWindow
{
	public:
		bool CreateThis(HWND hParent, u64 windowId);
		bool DestroyThis();
		void RefreshWindow(bool rebuildTree, bool refreshNames); // refreshNames is redundant/ignored when rebuildTree is set
		void FixPositions();

	protected:
		void CreateSubWindows();
		void EnableEditing();
		void DisableEditing();
		virtual void NotifyTreeSelChanged(LPARAM newValue); // Sent when a new tree item is selected
		virtual void NotifyButtonClicked(int idFrom, HWND hWndFrom); // Sent when a button or checkbox is clicked
		virtual void NotifyEditFocusLost(int idFrom, HWND hWndFrom);

	private:
		WinLib::TreeViewControl treeMiniViews;
		WinLib::GroupBoxControl groupSize, groupClip;
		WinLib::TextControl textWindowXc, textWindowYc, textWindowWidth, textWindowHeight;
		WinLib::TextControl textClipLeft, textClipTop, textClipRight, textClipBottom;
		WinLib::EditControl editWindowXc, editWindowYc, editWindowWidth, editWindowHeight;
		WinLib::EditControl editClipLeft, editClipTop, editClipRight, editClipBottom;
		WinLib::ButtonControl buttonMatchSourcePos, buttonMatchSourceSize;
		WinLib::ButtonControl buttonSetClip, buttonClearClip;
		WinLib::CheckBoxControl checkClipped, checkHideWhenSourceOnTop, checkLockSizeRatio;
		std::vector<std::tuple<HTREEITEM, std::string /* displayedMiniViewTitle */, std::shared_ptr<MiniView>>> currTreeItems;
		std::shared_ptr<MiniView> currMiniView;
};

#endif