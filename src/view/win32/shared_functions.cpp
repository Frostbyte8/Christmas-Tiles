#include "shared_functions.h"

bool CALLBACK ChangeControlsFont(HWND child, LPARAM font) {
    SendMessage(child, WM_SETFONT, font, TRUE);
    return true;
}

void CenterWindow(const HWND& hWnd, const RECT& windowRC, const HMONITOR& prevMonitor) {

    MONITORINFO monitorInfo = {0};
    monitorInfo.cbSize = sizeof(MONITORINFO);
    GetMonitorInfoW(prevMonitor, &monitorInfo);
  
    const int winWidth  = windowRC.right - windowRC.left; 
    const int winHeight = windowRC.bottom - windowRC.top;
    const int workXCenter = abs(monitorInfo.rcWork.left - monitorInfo.rcWork.right) / 2;
    const int workYCenter = abs(monitorInfo.rcWork.top - monitorInfo.rcWork.bottom) / 2;

    MoveWindow(hWnd, monitorInfo.rcWork.right - (workXCenter + (winWidth / 2)),
                     monitorInfo.rcWork.bottom - (workYCenter + (winHeight / 2)),
                     winWidth, winHeight, TRUE);
    /*
    SetWindowPos(hWnd, NULL,
                 monitorInfo.rcWork.right - (workXCenter + (winWidth / 2)), 
                 monitorInfo.rcWork.bottom - (workYCenter + (winHeight / 2)),
                 winWidth, winHeight, SWP_NOZORDER | SWP_);*/

}