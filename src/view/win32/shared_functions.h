#pragma once

#include <windows.h>

//==============================================================================
// Window Messages
//==============================================================================

#define UWM_TILE_SELECTED   (WM_USER + 100) // WPARAM = Tile Index Selected
#define UWM_SCORE_ENTERED   (WM_USER + 101) // Enter Score Window is closed

//==============================================================================
// Functions
//==============================================================================

//------------------------------------------------------------------------------
// createBackBuffer - Creates a back buffer for the context given. You must call
// destroyBackBuffer when you are done with the back buffer
// @param HDC to create the back buffer for
// @param HDC of the back buffer
// @param HBITMAP to be used for the back buffer
// @param HBITMAP to be used to hold the previous HBITMAP contained by the back buffer
// @param RECT dimensions of the back buffer
//------------------------------------------------------------------------------

__forceinline void createBackBuffer(const HDC& primaryContext, HDC& backBuffer, HBITMAP& backBitmap, HBITMAP& prevBitmap, const RECT& rc) {
    backBuffer      = CreateCompatibleDC(primaryContext);
    backBitmap      = CreateCompatibleBitmap(primaryContext, rc.right-rc.left, rc.bottom-rc.top);
    prevBitmap      = (HBITMAP)SelectObject(backBuffer, backBitmap);
}

//------------------------------------------------------------------------------
// destroyBackBuffer - Destroys a previously created back buffer.
// @param HDC of the back buffer
// @param HBITMAP used by the back buffer
// @param HBITMAP that the back buffer had when it was created
//------------------------------------------------------------------------------

__forceinline void destroyBackBuffer(HDC& backBuffer, HBITMAP& backBitmap, HBITMAP& prevBitmap) {
    SelectObject(backBuffer, prevBitmap);
    DeleteObject(backBitmap);
    DeleteDC(backBuffer);
}

//------------------------------------------------------------------------------
// ChangeControlsFont - Changes the font on the control given. Meant to be used
// with EnumChildWindows.
// @param HWND of the child window to change
// @param Font to be set
//------------------------------------------------------------------------------

bool CALLBACK ChangeControlsFont(HWND child, LPARAM font);

void CenterWindow(const HWND& hWnd, const RECT& windowRC, const HMONITOR& prevMonitor);