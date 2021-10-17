#ifndef ADVANCEDTAB_H
#define ADVANCEDTAB_H
#include "../../WindowsLib/WindowsUi.h"

class AdvancedWindow : public WinLib::ClassWindow
{
	public:
        AdvancedWindow();
		bool CreateThis(HWND hParent, u64 windowId, int dpi, HFONT font);
		bool DestroyThis();
		void RefreshWindow();
		void FixPositions(int dpi, HFONT font);
        bool LastActionClearedSettings();

	protected:
		void CreateSubWindows(int dpi, HFONT font);
		virtual void NotifyButtonClicked(int idFrom, HWND hWndFrom); // Sent when a button or checkbox is clicked
        virtual void NotifyEditUpdated(int idFrom, HWND hWndFrom); // Sent when edit text changes, before redraw

	private:
        bool blockEditNotify;
        bool lastActionClearedSettings;
		WinLib::CheckBoxControl checkShowDialogOnStart;
		WinLib::CheckBoxControl checkUseNotificationIcon;
		WinLib::CheckBoxControl checkMinimizeToNotificationIcon;
		WinLib::CheckBoxControl checkConfirmExit;
		WinLib::EditControl editDefaultOpacity;

		WinLib::GroupBoxControl groupViewDefaults;
		WinLib::CheckBoxControl checkDefaultMatchSourcePosition;
		WinLib::CheckBoxControl checkDefaultMatchSourceSize;
		WinLib::CheckBoxControl checkDefaultLockSizeRatio;
		WinLib::CheckBoxControl checkDefaultHideWhenSourceOnTop;

		WinLib::GroupBoxControl groupFrozenSourceBehaviorDefaults;
		WinLib::CheckBoxControl checkUseCachedImageWhenFrozen;
		WinLib::CheckBoxControl checkShowFrozenIndicatorIcon;
		WinLib::CheckBoxControl checkShowFrozenContextMenuItem;

		WinLib::ButtonControl buttonClearSavedSettings;
};

#endif