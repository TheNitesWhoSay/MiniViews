#ifndef GENERALTAB_H
#define GENERALTAB_H
#include "../../WindowsLib/WindowsUi.h"

class GeneralWindow : public WinLib::ClassWindow
{
	public:
		bool CreateThis(HWND hParent, u32 windowId);
		bool DestroyThis();
		void RefreshWindow();
		void FixPositions();

	protected:
		void CreateSubWindows();
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