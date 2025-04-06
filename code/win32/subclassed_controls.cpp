#include "subclassed_controls.h"

//=============================================================================
// Double Buffered Label - A label that should not flicker.
// Remember to use WM_SETREDRAW to disable redrawing when changing the text.
//=============================================================================

void doubleBufferedLabelOnPaint(HWND hWnd);

LRESULT CALLBACK doubleBufferedLabelProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    WNDPROC* oldLabelProc;
    LRESULT retVal;
    switch(msg) {

        default: 
            oldLabelProc = reinterpret_cast<WNDPROC*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
            return CallWindowProc((WNDPROC)oldLabelProc, hWnd, msg, wParam, lParam);
            
        case WM_PAINT: doubleBufferedLabelOnPaint(hWnd); break;
        case WM_ERASEBKGND: return (LRESULT)1;

        case WM_SETTEXT:
            oldLabelProc = reinterpret_cast<WNDPROC*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
            SendMessage(hWnd, WM_SETREDRAW, (WPARAM)FALSE, 0);
            retVal = CallWindowProc((WNDPROC)oldLabelProc, hWnd, msg, wParam, lParam);
            SendMessage(hWnd, WM_SETREDRAW, (WPARAM)TRUE, 0);
            return retVal;

    }

    return 0;
}

void doubleBufferedLabelOnPaint(HWND hWnd) {

    RECT rc;
    HDC hdc;
    PAINTSTRUCT ps;
    HDC hdcMem;
    HBITMAP hbmMem, hbmOld;
    HBRUSH hbrBkGnd;
    HFONT hfntOld = NULL;

    const int MAX_STR_SIZE = 128;

    wchar_t timeStr[MAX_STR_SIZE] = {0};
    GetWindowTextW(hWnd, timeStr, MAX_STR_SIZE);

    hdc = BeginPaint(hWnd, &ps);

    GetClientRect(hWnd, &rc);

    hdcMem = CreateCompatibleDC(ps.hdc);
    hbmMem = CreateCompatibleBitmap(ps.hdc, rc.right-rc.left, rc.bottom-rc.top);

    hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);
    hbrBkGnd = CreateSolidBrush(GetSysColor(COLOR_3DFACE));
    FillRect(hdcMem, &rc, hbrBkGnd);
    DeleteObject(hbrBkGnd);

    HFONT hfnt = (HFONT)SendMessage(hWnd, WM_GETFONT, 0, 0);

    if(hfnt) {
        hfntOld = (HFONT)SelectObject(hdcMem, hfnt);
    }

    SetBkMode(hdcMem, TRANSPARENT);
    SetTextColor(hdcMem, GetSysColor(COLOR_WINDOWTEXT));
    DrawText(hdcMem, timeStr, -1, &rc, DT_CENTER);
    if(hfntOld) {
        SelectObject(hdcMem, hfntOld);
    }

    BitBlt(ps.hdc, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, hdcMem, 0, 0, SRCCOPY);

    SelectObject(hdcMem, hbmOld);
    DeleteObject(hbmMem);
    DeleteDC(hdcMem);
    EndPaint(hWnd, &ps);
}
