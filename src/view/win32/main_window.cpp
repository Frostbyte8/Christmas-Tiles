#include "main_window.h"
#include "subclassed_controls.h"

static const DWORD WINDOW_STYLE = WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_THICKFRAME);
static const DWORD WINDOW_STYLE_EX = WS_EX_OVERLAPPEDWINDOW;

//==============================================================================
// Namespaces / Enums
//==============================================================================

namespace CtrlID {
    enum CtrlID {
        GROUP_SCORE     = 101,
        GROUP_POINTS,
        GROUP_TIME,
        LABEL_SCORE,
        LABEL_POINTS,
        LABEL_TIME,
        BUTTON_PAUSE,
        PANEL_GAMEBOARD,
    };
}

//==============================================================================
// Inline Functions
//==============================================================================

__forceinline HWND createGroupBox(const wchar_t* title, const HWND& parent, const int& ID, const HINSTANCE& hInst) {
    return CreateWindowEx(0, L"BUTTON", title, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_GROUPBOX, 
                          0, 0, 0, 0, parent, reinterpret_cast<HMENU>(ID), hInst, NULL);
}

__forceinline HWND createButton(const wchar_t* title, const HWND& parent, const int& ID, const HINSTANCE& hInst) {
    return CreateWindowEx(0, L"BUTTON", title, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_PUSHBUTTON, 
                          0, 0, 0, 0, parent, reinterpret_cast<HMENU>(ID), hInst, NULL);
}

//==============================================================================
// Public Functions
//==============================================================================

//==============================================================================
// Win32 Messages
//==============================================================================

//------------------------------------------------------------------------------
// onCreate - Processes the WM_CREATE message
//------------------------------------------------------------------------------

bool MainWindowView::onCreate() {

    static const DWORD DEF_STYLE_FLAGS = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS;
    
    groupStats[0] = createGroupBox(L"Score", hWnd, CtrlID::GROUP_SCORE, hInstance);
    groupStats[1] = createGroupBox(L"Points", hWnd, CtrlID::GROUP_POINTS, hInstance);
    groupStats[2] = createGroupBox(L"Time", hWnd, CtrlID::GROUP_TIME, hInstance);

    scoreLabel.createWindow(hInstance, L"000000000", hWnd, reinterpret_cast<HMENU>(CtrlID::LABEL_SCORE));
    pointsLabel.createWindow(hInstance, L"0", hWnd, reinterpret_cast<HMENU>(CtrlID::LABEL_POINTS));
    timeLabel.createWindow(hInstance, L"00:00", hWnd, reinterpret_cast<HMENU>(CtrlID::LABEL_TIME));

    buttonPause = createButton(L"Pause", hWnd, CtrlID::BUTTON_PAUSE, hInstance);
    
    gamePanel.createWindow(hInstance, hWnd, reinterpret_cast<HMENU>(CtrlID::PANEL_GAMEBOARD));
    metrics.initWindowMetrics();

    HFONT dialogFont = metrics.GetCurrentFont();
    EnumChildWindows(hWnd, reinterpret_cast<WNDENUMPROC>(SetProperFont), (LPARAM)dialogFont);

    prevMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);

    moveControls();
    return true;

}


//------------------------------------------------------------------------------
// getTallestPoint - Obtain the tallest point of the window.
//------------------------------------------------------------------------------

// TODO: Tile Size, etc.

LONG MainWindowView::getTallestPoint() const {
    const ControlDimensions& CD = metrics.getControlDimensions();
    const ControlSpacing& CS = metrics.getControlSpacing();

    const LONG minHeight = 288;

    LONG tallestPoint = (CD.YLABEL * 3) + (CS.YFIRST_GROUPBOX_MARGIN * 3) + (CS.YLAST_GROUPBOX_MARGIN * 3) +
                        (CD.YBUTTON);
    
    return (tallestPoint < minHeight) ? minHeight : tallestPoint;
}

//------------------------------------------------------------------------------
// getWidestGroupBox - Identify which Groupbox is the widest
//------------------------------------------------------------------------------

LONG MainWindowView::getWidestGroupBox() const {

    // TODO: Actually write function properly

    //const ControlDimensions& CD = metrics.getControlDimensions();
    const ControlSpacing& CS = metrics.getControlSpacing();

    wchar_t str[11] = L"000000000\0";
    LONG widestGroupBox = metrics.calculateStringWidth(str);
    widestGroupBox += CS.XGROUPBOX_MARGIN * 2;
    return widestGroupBox;

}

//------------------------------------------------------------------------------
// moveControls - Move/Resize Controls to their proper positions, and then
// centers the window as this only happens when you change the board size
//------------------------------------------------------------------------------

void MainWindowView::moveControls() {

    const ControlDimensions& CD = metrics.getControlDimensions();
    const ControlSpacing& CS = metrics.getControlSpacing();

    // Obtain Client Area Width
    // Client Area Width is Widest GroupBox + GameBoard Width
    // Client Area Height is whatever is taller: 3 Groupboxes + Button, or Gameboard
    // Min Width: GroupBox + at least 5 tiles
    // Min Height: No less than 9 tiles tall.

    // TODO: Scroll Bars when the width/height gets too wide.
    const LONG tallestPoint = getTallestPoint();
    const LONG widestGroupBox = getWidestGroupBox();

    // TODO: This may not need to actually be here
    // TODO: Min Height = the Height of Standard Tile at current dpi * 9 (32 * 9 for 96, 48 * 9 for 144, etc)

    // Resize Window
    // TODO: Get Monitor Window is on?

    RECT rc = {0, 0, (32 * 5) + widestGroupBox, tallestPoint};
    AdjustWindowRectEx(&rc, WINDOW_STYLE, FALSE, WINDOW_STYLE_EX);
    MoveWindow(hWnd, 0, 0, rc.right-rc.left, rc.bottom-rc.top, FALSE);

    const LONG boxHeight = CD.YLABEL + CS.YFIRST_GROUPBOX_MARGIN + CS.YLAST_GROUPBOX_MARGIN;
    
    // Move Controls into position

    HDWP hDeferedWindows = BeginDeferWindowPos(8);

    DeferWindowPos(hDeferedWindows, groupStats[0], HWND_NOTOPMOST, 0, 0, widestGroupBox, boxHeight, SWP_NOZORDER);
    DeferWindowPos(hDeferedWindows, groupStats[1], HWND_NOTOPMOST, 0, boxHeight, widestGroupBox, boxHeight, SWP_NOZORDER);
    DeferWindowPos(hDeferedWindows, groupStats[2], HWND_NOTOPMOST, 0, boxHeight*2, widestGroupBox, boxHeight, SWP_NOZORDER);
    
    DeferWindowPos(hDeferedWindows, scoreLabel.getHandle(), HWND_NOTOPMOST, CS.XGROUPBOX_MARGIN, CS.YFIRST_GROUPBOX_MARGIN, widestGroupBox - (CS.XGROUPBOX_MARGIN * 2), CD.YLABEL, SWP_NOZORDER);
    DeferWindowPos(hDeferedWindows, pointsLabel.getHandle(), HWND_NOTOPMOST, CS.XGROUPBOX_MARGIN, CS.YFIRST_GROUPBOX_MARGIN + (boxHeight), widestGroupBox - (CS.XGROUPBOX_MARGIN * 2), CD.YLABEL, SWP_NOZORDER);
    DeferWindowPos(hDeferedWindows, timeLabel.getHandle(), HWND_NOTOPMOST, CS.XGROUPBOX_MARGIN, CS.YFIRST_GROUPBOX_MARGIN + (boxHeight * 2), widestGroupBox - (CS.XGROUPBOX_MARGIN * 2), CD.YLABEL, SWP_NOZORDER);
    
    DeferWindowPos(hDeferedWindows, buttonPause, HWND_NOTOPMOST, 0, boxHeight * 3, widestGroupBox, tallestPoint - (boxHeight * 3), SWP_NOZORDER);
    DeferWindowPos(hDeferedWindows, gamePanel.getHandle(), HWND_NOTOPMOST, widestGroupBox, 0, 160, 288, SWP_NOZORDER);

    EndDeferWindowPos(hDeferedWindows);

    centerWindow();
    
}

//==============================================================================
// Private Functions
//==============================================================================

//-----------------------------------------------------------------------------
// centerWindow - Centers the window on the monitor it was said to have last
// been on.
//-----------------------------------------------------------------------------

void MainWindowView::centerWindow() {

    MONITORINFO monitorInfo = {0};
    monitorInfo.cbSize = sizeof(monitorInfo);
    GetMonitorInfoW(prevMonitor, &monitorInfo);

    RECT windowRC;
    GetWindowRect(hWnd, &windowRC);

    const int winWidth  = windowRC.right - windowRC.left; 
    const int winHeight = windowRC.bottom - windowRC.top;
    const int workXCenter = abs(monitorInfo.rcWork.left - monitorInfo.rcWork.right) / 2;
    const int workYCenter = abs(monitorInfo.rcWork.top - monitorInfo.rcWork.bottom) / 2;

    SetWindowPos(hWnd, NULL,
                 monitorInfo.rcWork.right - (workXCenter + (winWidth / 2)), 
                 monitorInfo.rcWork.bottom - (workYCenter + (winHeight / 2)),
                 winWidth, winHeight, SWP_NOREDRAW);

}

//------------------------------------------------------------------------------
// createWindow - Creates the main Window.
//------------------------------------------------------------------------------

bool MainWindowView::createWindow() {

    if(hWnd) {
        return true; // Already created.
    }

    // TODO: This may need WS_CLIPCHILDREN?

    hWnd = CreateWindowEx(WINDOW_STYLE_EX, L"XmasTilesMainWindow", L"",
        WINDOW_STYLE, CW_USEDEFAULT, CW_USEDEFAULT, 200, 200,
        NULL, NULL, hInstance, this);

    if(hWnd == NULL) {
        MessageBox(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return false;
    }

    ShowWindow(hWnd, SW_NORMAL);
    UpdateWindow(hWnd);

    startTime = GetTickCount();
    SetTimer(hWnd, 1, 100, 0);

    return true;
}

//------------------------------------------------------------------------------
// doLoop - Standard run of the mill message loop
//------------------------------------------------------------------------------

UINT MainWindowView::doLoop() {
    
    MSG msg;
    
    while(GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

//------------------------------------------------------------------------------
// registerSelf - Registers the Window class.
//------------------------------------------------------------------------------

bool MainWindowView::registerSelf() {
    
    WNDCLASSEX wc;

    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = MainWindowView::WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = L"XmasTilesMainWindow";
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassEx(&wc)) {
        MessageBox(NULL, L"Error registering window class.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
// onTileSelected - Called when a tile was selected.
//------------------------------------------------------------------------------

void MainWindowView::onTileSelected(const WORD& tileIndex) {
    wchar_t outStr[256] = {0};
    wsprintf(outStr, L"%d\n", tileIndex);
    OutputDebugString(outStr);
}

//------------------------------------------------------------------------------
// OnTimer - Processes the WM_TIMER Event
//------------------------------------------------------------------------------

void MainWindowView::onTimer() {
    
    elapsedTime = gamePanel.windowPresenter.getElapsedTime();

    DWORD seconds = elapsedTime / 1000;
    DWORD minutes = seconds / 60;
    seconds = seconds - (minutes * 60);

    wchar_t timeStr[32] = {0};
    wsprintf(timeStr, L"%02d:%02d", minutes, seconds);
    SetWindowText(timeLabel.getHandle(), timeStr);


}

//------------------------------------------------------------------------------
// windowProc - Standard window procedure for a window
//------------------------------------------------------------------------------

LRESULT MainWindowView::windowProc(const UINT& msg, const WPARAM wParam, const LPARAM lParam) {

    switch(msg) {
        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);

        case WM_TIMER:
            onTimer();
            break;

        case WM_CREATE:
            onCreate();
            break;

        case UWM_TILE_SELECTED:
            onTileSelected(static_cast<WORD>(wParam));
            break;

        case WM_CLOSE:
            KillTimer(hWnd, 1);
            DestroyWindow(hWnd); 
            break;
        case WM_DESTROY: 
            PostQuitMessage(0);
            break;
    }
    return 0;
}

