#ifndef ADVANCEDTAB_H
#define ADVANCEDTAB_H
#include "../../WindowsLib/WindowsUi.h"

class AdvancedWindow : public WinLib::ClassWindow
{
	public:
        AdvancedWindow();
		bool CreateThis(HWND hParent, u32 windowId);
		bool DestroyThis();
		void RefreshWindow();
		void FixPositions();
        bool LastActionClearedSettings();

	protected:
		void CreateSubWindows();
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

		WinLib::ButtonControl buttonClearSavedSettings;
};

#endif