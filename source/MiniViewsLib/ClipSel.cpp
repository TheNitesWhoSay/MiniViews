#include "ClipSel.h"
#include "MiniViews.h"

ClipSel::ClipSel() : startX(0), startY(0), hOutline(NULL), backgroundBrush(NULL), regionSelectCursor(NULL), miniView(nullptr)
{
	rcClip.left = 0;
	rcClip.top = 0;
	rcClip.right = 0;
	rcClip.bottom = 0;
	hOutline = ::CreateSolidBrush(RGB(190, 200, 255));
	backgroundBrush = ::CreateSolidBrush(RGB(255, 255, 255));
	regionSelectCursor = ::LoadCursor(NULL, IDC_CROSS);
}

ClipSel::~ClipSel()
{
	::DeleteObject(hOutline);
	::DeleteObject(backgroundBrush);
	::DestroyCursor(regionSelectCursor);
}

bool ClipSel::CreateThis(HWND hParent, MiniView &miniViewToClip)
{
	this->miniView = &miniViewToClip;
	RECT rcSource = {}, rcSourceCli = {};
	::GetWindowRect(miniView->GetSourceWindow(), &rcSource);
	::GetClientRect(miniView->GetSourceWindow(), &rcSourceCli);
	POINT cliTopLeft = {};
	cliTopLeft.x = rcSourceCli.left;
	cliTopLeft.y = rcSourceCli.top;
	::ClientToScreen(miniView->GetSourceWindow(), &cliTopLeft);

	if ( ClassWindow::RegisterWindowClass(NULL, NULL, regionSelectCursor, backgroundBrush, NULL, "ClipSelWindow", NULL, false) &&
		 ClassWindow::CreateClassWindow(NULL, "Select Clip", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
			 cliTopLeft.x, cliTopLeft.y, rcSourceCli.right - rcSourceCli.left, rcSourceCli.bottom - rcSourceCli.top, hParent, NULL) )
	{
		::SetForegroundWindow(getHandle());
		WindowsItem::LockCursor();
		return true;
	}
	return false;
}

void ClipSel::DestroyThis()
{
	ClassWindow::DestroyThis();
	startX = 0;
	startY = 0;
	rcClip.left = 0;
	rcClip.top = 0;
	rcClip.right = 0;
	rcClip.bottom = 0;
	miniView = nullptr;
}

void ClipSel::LButtonDown(int xc, int yc)
{
	startX = xc;
	startY = yc;
	rcClip.left = xc;
	rcClip.top = yc;
}

void ClipSel::MouseMove(WPARAM wParam, int xc, int yc)
{
	if ( (wParam & MK_LBUTTON) == MK_LBUTTON )
	{
		rcClip.left = std::min(startX, (LONG)xc);
		rcClip.top = std::min(startY, (LONG)yc);
		rcClip.right = std::max(startX, (LONG)xc);
		rcClip.bottom = std::max(startY, (LONG)yc);
		WindowsItem::RedrawThis();
	}
}

void ClipSel::LButtonUp(int xc, int yc)
{
	rcClip.left = std::min(startX, (LONG)xc);
	rcClip.top = std::min(startY, (LONG)yc);
	rcClip.right = std::max(startX, (LONG)xc);
	rcClip.bottom = std::max(startY, (LONG)yc);

	WindowsItem::UnlockCursor();
	miniViews.EndSetClipRegion(*miniView, rcClip);

	rcClip.left = 0;
	rcClip.top = 0;
	rcClip.right = 0;
	rcClip.bottom = 0;
}

void ClipSel::Paint()
{
	WindowsItem::StartBufferedPaint();
	WindowsItem::FillPaintArea(backgroundBrush);
	WindowsItem::FrameRect(hOutline, rcClip);
	WindowsItem::EndPaint();
}

LRESULT ClipSel::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case WM_LBUTTONDOWN: LButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;
		case WM_MOUSEMOVE: MouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;
		case WM_LBUTTONUP: LButtonUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;
		case WM_PAINT: Paint(); break;
		default: return ClassWindow::WndProc(hWnd, msg, wParam, lParam); break;
	}
	return 0;
}
