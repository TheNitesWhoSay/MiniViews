#ifndef MINIVIEWS_H
#define MINIVIEWS_H
#include "../WindowsLib/WindowsUi.h"
#include "MainDialog/MainDialog.h"
#include "MiniView.h"
#include "ClipSel.h"
#include "Updater.h"
#include "Preferences.h"
#include <vector>
#include <atomic>

// MultiMontior... GetSystemMetrics(SM_CMONITORS/SM_CXVIRTUALSCREEN/SM_CYVIRTUALSCREEN/SM_XVIRTUALSCREEN/SM_YVIRTUALSCREEN)
//                 GetDeviceCaps

class MiniViews : public WinLib::ClassWindow, public IUpdatable, public IMiniViewUser
{
	public:

        Preferences prefs;

		MiniViews();
		~MiniViews();

		int Run(int showCommand); // Starts the program using the provided arguments
		int CriticalError(int returnValue, const std::string & errorMessage);

		virtual void TimedUpdate(); // Causes UpdateMiniViews to run soon on the main thread
		std::vector<std::shared_ptr<MiniView>> &GetMiniViewList();
		void EndSetClipRegion(MiniView &miniView, RECT rcClip);
		bool IsInEditMode();
		void ToggleEditMode();
		void AddMiniView();
		void ClearMiniViews();
		u8 GetOpacityLevel();
		void SetOpacityLevel(u8 newOpacityLevel);

        bool CreateNotificationIcon();
		void RemoveNotificationIcon();


	protected:

        void Rest(); // Called when no MiniViews are active to reduce resource use
        void Wake(); // Called when a MiniView is added after Rest() has been called

		virtual void NotifyChange(MiniView &miniView); // Sent when the user causes a change to the MiniView
		virtual void CloseMiniView(MiniView &miniView); // Closes the corresponding MiniView
		virtual void SetClipRegion(MiniView &miniView);
        virtual bool GetDefaultMatchSourcePosition(MiniView &miniView);
        virtual bool GetDefaultMatchSourceSize(MiniView &miniView);
        virtual bool GetDefaultLockSizeRatio(MiniView &miniView);
        virtual bool GetDefaultHideWhenSourceOnTop(MiniView &miniView);
		virtual bool GetUseCachedImageWhenFrozen(MiniView &miniView);
		virtual bool GetShowFrozenIndicatorIcon(MiniView &miniView);
		virtual bool GetShowFrozenContextMenuItem(MiniView &miniView);
		bool CreateThis(); // Creates this window
		bool RegisterHotkeys(); // Registers the relevant hotkeys for controlling the program
		bool EnsureOnlyInstance();

		void RunFrame(); // Fires once every 25ms to ensure operations required on a regular basis occur
		void RunHalfSecondActions();

		bool isTransparencyMinimum();
		bool isTransparencyMax();

		void IncrementAlpha();
		void DecrementAlpha();

		void FixPosition();
		bool HasNotificationIcon();
		void RunNotificationContextMenu();
		LRESULT HotkeyPressed(WPARAM wParam, LPARAM lParam);
		LRESULT IconNotify(WPARAM wParam, LPARAM lParam);
		LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


	private:

		ClipSel clipSel;
		MainDialog mainDialog;
		HCURSOR defaultCursor;
		HICON smallIcon, mediumIcon;
		COLORREF transparentColor; // The color which is set to transparent for this window
		HBRUSH transparentBrush; // A brush with the transparent color
		u8 transparencyAlpha; // The transparency alpha level, lower levels are more see through
		bool editMode; // Whether the program currently allows individual MiniViews to be edited
		bool hasAutoDisplayedPage; // Whether the program has auto displayed a page this run
		int frameNumber; // The frame number the program is on, cycles from 0 to framesPerCycle-1
		std::vector<std::shared_ptr<MiniView>> currMiniViews; // A list of all active MiniViews
		std::atomic<bool> hasFrameToRun; // When true a frame will occur shortly
		Updater updater; // The automatic updater for this collection of MiniViews

		static const int msPerFrame; // The minimum amount of time between two frames
		static const int framesPerCycle; // Max time between regularly scheduled events divided by msPerFrame
		static const u32 notificationId; // A unique identifier for this applications notification icon
		static const std::string mainClassName; // The name of MiniView's window class
        public: static const std::string mainWindowName; private: // The name associated with MiniView's main window
};

/** The main instance of MiniViews.
	By signaling that miniViews is declared externally here,
	arbitrary .cpp files can include this header and
	access various parts of the program as needed */
extern MiniViews miniViews; // The main instance of MiniViews

void RefreshNotificationIconArea();

#endif