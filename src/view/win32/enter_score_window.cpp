#include "enter_score_window.h"
#include "../../language_table.h"
#include "shared_functions.h"
#include "../../model/scores.h"

#ifdef _DPI_AWARE_
#include <shellscalingapi.h>
#endif // _DPI_AWARE_

#include <assert.h>


bool EnterScoreWindow::isRegistered = false;

static const DWORD WINDOW_STYLE = WS_POPUPWINDOW | WS_CAPTION | WS_DLGFRAME | DS_MODALFRAME;
static const DWORD WINDOW_STYLE_EX = WS_EX_DLGMODALFRAME;

__forceinline HWND createLabel(const wchar_t* title, const HWND& parent, const int& ID, const HINSTANCE& hInst) {
    return CreateWindowEx(0, L"Static", title, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | SS_CENTER, 
                          0, 0, 0, 0, parent, reinterpret_cast<HMENU>(ID), hInst, NULL);
}

__forceinline HWND createButton(const wchar_t* title, const HWND& parent, const int& ID, const HINSTANCE& hInst) {
    return CreateWindowEx(0, L"BUTTON", title, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_DEFPUSHBUTTON, 
                          0, 0, 0, 0, parent, reinterpret_cast<HMENU>(ID), hInst, NULL);
}

namespace CtrlID {
    enum CtrlID {
        SCORE_TEXT_1 = 101,
        SCORE_TEXT_2,
        NAME_ENTRY_BOX,
        OK_BUTTON,
    };
}

//-----------------------------------------------------------------------------
// createWindow - Creates the about Window
//-----------------------------------------------------------------------------

bool EnterScoreWindow::createWindow(const HINSTANCE& hInst, const HWND& parent, const HMONITOR& parentMonitor, size_t& inScoreIndex) {

    scoreIndex = inScoreIndex;

    if(hWnd) {
        return true; // Already created.
    }

    parentHWnd = parent;
    prevMonitor = parentMonitor;

    MONITORINFO monitorInfo = {0};
    monitorInfo.cbSize = sizeof(MONITORINFO);
    GetMonitorInfo(prevMonitor, &monitorInfo);

    hWnd = CreateWindowEx(WINDOW_STYLE_EX, L"EnterScoreWindow", GET_LANG_STR(LangID::ENTER_SCORE_WINDOW_TITLE),
        WINDOW_STYLE, (monitorInfo.rcWork.left - monitorInfo.rcWork.right), (monitorInfo.rcWork.top - monitorInfo.rcWork.bottom), 0, 0,
        parent, NULL, hInst, this);

    if(hWnd == NULL) {
        MessageBox(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return false;
    }

    EnableWindow(parent, FALSE);
    
    return true;

}

//------------------------------------------------------------------------------
// registerSelf - Registers the Window class.
//------------------------------------------------------------------------------

bool EnterScoreWindow::registerSelf(const HINSTANCE& hInst) {
    
    WNDCLASSEX wc;

    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = EnterScoreWindow::WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInst;
    wc.hIcon         = NULL;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = L"EnterScoreWindow";
    wc.hIconSm       = NULL;

    if(!RegisterClassEx(&wc)) {
        MessageBox(NULL, L"Error registering window class.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// onCreate - Processes the WM_CREATE message
//-----------------------------------------------------------------------------

void EnterScoreWindow::onCreate() {
    const HINSTANCE hInst = GetModuleHandle(NULL);

    strCache[0] = GET_LANG_STR(LangID::ENTER_SCORE_MESSAGE_1);
    strCache[1] = GET_LANG_STR(LangID::ENTER_SCORE_MESSAGE_2);

    labelMessage1 = createLabel(strCache[0], hWnd, CtrlID::SCORE_TEXT_1, hInst);
    labelMessage2 = createLabel(strCache[1], hWnd, CtrlID::SCORE_TEXT_2, hInst);

    textYourName = CreateWindowEx(WS_EX_CLIENTEDGE, L"Edit", L"", WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_CENTER, 0, 0, 0, 0, hWnd, (HMENU)CtrlID::NAME_ENTRY_BOX, hInst, 0);

    SendMessage(textYourName, EM_SETLIMITTEXT, ScoreTableConstants::MAX_NAME_LENGTH, 0);

    buttonOK = createButton(GET_LANG_STR(LangID::OK_BUTTON_CAPTION), hWnd, CtrlID::OK_BUTTON, hInst);
    SendMessage(hWnd, DM_SETDEFID, CtrlID::OK_BUTTON, 0);

#ifdef _DPI_AWARE_
    UINT xDPI;
    UINT yDPI;
    GetDpiForMonitor(prevMonitor, MONITOR_DPI_TYPE::MDT_DEFAULT, &xDPI, &yDPI);
    onDPIChange(static_cast<float>(xDPI), static_cast<float>(yDPI));
#else
    metrics.initWindowMetrics();
    HFONT dialogFont = metrics.GetCurrentFont();
    EnumChildWindows(hWnd, reinterpret_cast<WNDENUMPROC>(ChangeControlsFont), (LPARAM)dialogFont);
#endif // _DPI_AWARE_

    moveControls();
    windowMoving = false;
    ShowWindow(hWnd, SW_NORMAL);
    UpdateWindow(hWnd);
}

//------------------------------------------------------------------------------
// onDPIChange - Processes the WM_DPICHANGED messages
//------------------------------------------------------------------------------

void EnterScoreWindow::onDPIChange(const float xDPI, const float yDPI) {
    metrics.initWindowMetrics(xDPI, yDPI);
    HFONT dialogFont = metrics.GetCurrentFont();
    EnumChildWindows(hWnd, reinterpret_cast<WNDENUMPROC>(ChangeControlsFont), (LPARAM)dialogFont);
    if(!windowMoving) {
        moveControls();
    }
}

void EnterScoreWindow::moveControls() {

    assert(strCache[0] != NULL);
    assert(strCache[1] != NULL);

    const ControlDimensions& CD = metrics.getControlDimensions();
    const ControlSpacing& CS = metrics.getControlSpacing();

    const LONG totalWidth = max(metrics.calculateStringWidth(strCache[0]), metrics.calculateStringWidth(strCache[1])) + (CS.XWINDOW_MARGIN * 2);
    const LONG totalHeight = (CS.YWINDOW_MARGIN * 2) + (CD.YLABEL * 2) + (CD.YTEXTBOX_ONE_LINE_ALONE) + (CD.YBUTTON) + (CS.YRELATED_MARGIN * 3);

    HDWP hDeferedWindows = BeginDeferWindowPos(4);

    int yOffset = CS.YWINDOW_MARGIN;
    hDeferedWindows = DeferWindowPos(hDeferedWindows, labelMessage1, HWND_NOTOPMOST, CS.XWINDOW_MARGIN, yOffset, totalWidth - (CS.XWINDOW_MARGIN * 2), CD.YLABEL, SWP_NOZORDER);
    yOffset += CD.YLABEL + CS.YRELATED_MARGIN;
    hDeferedWindows = DeferWindowPos(hDeferedWindows, labelMessage2, HWND_NOTOPMOST, CS.XWINDOW_MARGIN, yOffset, totalWidth - (CS.XWINDOW_MARGIN * 2), CD.YLABEL, SWP_NOZORDER);
    yOffset += CD.YLABEL + CS.YRELATED_MARGIN;
    hDeferedWindows = DeferWindowPos(hDeferedWindows, textYourName, HWND_NOTOPMOST, CS.XWINDOW_MARGIN, yOffset, totalWidth - (CS.XWINDOW_MARGIN * 2), CD.YTEXTBOX_ONE_LINE_ALONE, SWP_NOZORDER);
    yOffset += CD.YTEXTBOX_ONE_LINE_ALONE + CS.YRELATED_MARGIN;
    hDeferedWindows = DeferWindowPos(hDeferedWindows, buttonOK, HWND_NOTOPMOST, (totalWidth / 2) - (CD.XBUTTON / 2), yOffset, CD.XBUTTON, CD.YBUTTON, SWP_NOZORDER);

    EndDeferWindowPos(hDeferedWindows);

    RECT rc = {0, 0, totalWidth, totalHeight};
    AdjustWindowRectEx(&rc, WINDOW_STYLE, FALSE, WINDOW_STYLE_EX);
    CenterWindow(hWnd, rc, prevMonitor);

}

//-----------------------------------------------------------------------------
// onWindowMoved - Adjust the window, if needed after is has been moved.
//-----------------------------------------------------------------------------

void EnterScoreWindow::onWindowMoved() {
    HMONITOR currentMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);

    if(currentMonitor != prevMonitor) {
        prevMonitor = currentMonitor;
        moveControls(); // This will also center it
        RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
    }

}

//-----------------------------------------------------------------------------
// windowProc - Standard window procedure for a window
//-----------------------------------------------------------------------------

LRESULT EnterScoreWindow::windowProc(const UINT& msg, const WPARAM wParam, const LPARAM lParam) {

    switch(msg) {
        
        default: return DefWindowProc(hWnd, msg, wParam, lParam);

        case WM_CREATE:
            onCreate();
            break;
        case WM_ENTERSIZEMOVE:
            windowMoving = true;
            break;

        case WM_EXITSIZEMOVE:
            windowMoving = false;
            onWindowMoved();
            break;

        case DM_GETDEFID:
            return MAKEWPARAM(CtrlID::OK_BUTTON, DC_HASDEFID);

        case WM_COMMAND:
            
            if(wParam != CtrlID::OK_BUTTON) {
                return DefWindowProc(hWnd, msg, wParam, lParam);
            }

        case WM_CLOSE:
            SendMessage(parentHWnd, UWM_DIALOG_CLOSED, 0, 0);
            if(name) {
                free(name);
                name = NULL;
            }

            name = (wchar_t*)malloc(sizeof(wchar_t) * (ScoreTableConstants::MAX_NAME_LENGTH + 1));
            name[ScoreTableConstants::MAX_NAME_LENGTH] = 0;
            GetWindowText(textYourName, name, ScoreTableConstants::MAX_NAME_LENGTH+1);

            EnableWindow(parentHWnd, TRUE);
            SetFocus(parentHWnd);
            DestroyWindow(hWnd);
            SendMessage(parentHWnd, UWM_SCORE_ENTERED, 0, 0);
            hWnd = NULL;
            break;
#ifdef _DPI_AWARE_
        case WM_DPICHANGED:
            onDPIChange(LOWORD(wParam), HIWORD(wParam));
            break;
#endif // #ifdef _DPI_AWARE_
    }

    return 0;
}