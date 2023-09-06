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
        bool CreateThis(HWND hParent, u64 windowId, int dpi, HFONT font);
        bool DestroyThis();
        void RefreshWindow(bool rebuildTree, bool refreshNames); // refreshNames is redundant/ignored when rebuildTree is set
        void FixPositions(int dpi, HFONT font);

    protected:
        void CreateSubWindows(int dpi, HFONT font);
        void EnableEditing();
        void DisableEditing();
        void NotifyTreeItemSelected(LPARAM newValue) override; // Sent when a tree item is selected, or an already-selected item is re-clicked
        void NotifyButtonClicked(int idFrom, HWND hWndFrom) override; // Sent when a button or checkbox is clicked
        void NotifyEditFocusLost(int idFrom, HWND hWndFrom) override;

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