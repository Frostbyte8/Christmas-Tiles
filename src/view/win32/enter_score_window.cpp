#include "enter_score_window.h"
#include "../../language_table.h"
#include "shared_functions.h"

bool EnterScoreWindow::isRegistered = false;

static const DWORD WINDOW_STYLE = WS_POPUPWINDOW | WS_CAPTION | WS_DLGFRAME | DS_MODALFRAME;
static const DWORD WINDOW_STYLE_EX = WS_EX_DLGMODALFRAME;

// TODO: Until LangIDs are all finished
static const wchar_t tempCaption[] = L"Congratulations! You achieved a new highscore!";
static const wchar_t tempCaption2[] = L"Enter your name below";

__forceinline HWND createLabel(const wchar_t* title, const HWND& parent, const int& ID, const HINSTANCE& hInst) {
    return CreateWindowEx(0, L"Static", title, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | SS_CENTER, 
                          0, 0, 0, 0, parent, reinterpret_cast<HMENU>(ID), hInst, NULL);
}

__forceinline HWND createButton(const wchar_t* title, const HWND& parent, const int& ID, const HINSTANCE& hInst) {
    return CreateWindowEx(0, L"BUTTON", title, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_PUSHBUTTON, 
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

bool EnterScoreWindow::createWindow(const HINSTANCE& hInst, const HWND& parent) {

    if(hWnd) {
        return true; // Already created.
    }

    parentHWnd = parent;

    hWnd = CreateWindowEx(WINDOW_STYLE_EX, L"EnterScoreWindow", L"Add Title Here",
        WINDOW_STYLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 320, 320,
        parent, NULL, hInst, this);

    if(hWnd == NULL) {
        MessageBox(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return false;
    }

    EnableWindow(parent, FALSE);
    
    // TODO: Center window, monitor info, etc
    ShowWindow(hWnd, SW_NORMAL);
    UpdateWindow(hWnd);

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

    labelMessage1 = createLabel(tempCaption, hWnd, CtrlID::SCORE_TEXT_1, hInst);
    labelMessage2 = createLabel(tempCaption2, hWnd, CtrlID::SCORE_TEXT_2, hInst);

    textYourName = CreateWindowEx(WS_EX_CLIENTEDGE, L"Edit", L"", WS_CHILD | WS_VISIBLE | ES_CENTER, 0, 0, 0, 0, hWnd, (HMENU)CtrlID::NAME_ENTRY_BOX, hInst, 0);
    buttonOK = createButton(L"OK", hWnd, CtrlID::OK_BUTTON, hInst);

    metrics.initWindowMetrics();
    HFONT dialogFont = metrics.GetCurrentFont();
    EnumChildWindows(hWnd, reinterpret_cast<WNDENUMPROC>(ChangeControlsFont), (LPARAM)dialogFont);

    moveControls();
}

void EnterScoreWindow::moveControls() {

    const ControlDimensions& CD = metrics.getControlDimensions();
    const ControlSpacing& CS = metrics.getControlSpacing();

    const LONG totalWidth = max(metrics.calculateStringWidth(tempCaption), metrics.calculateStringWidth(tempCaption2)) + (CS.XWINDOW_MARGIN * 2);
    const LONG totalHeight = (CS.YWINDOW_MARGIN * 2) + (CD.YLABEL * 2) + (CD.YTEXTBOX_ONE_LINE_ALONE) + (CD.YBUTTON) + (CS.YRELATED_MARGIN * 3);

    RECT rc = {0, 0, totalWidth, totalHeight};
    AdjustWindowRectEx(&rc, WINDOW_STYLE, FALSE, WINDOW_STYLE_EX);
    MoveWindow(hWnd, 0, 0, rc.right-rc.left, rc.bottom-rc.top, FALSE);
    
    HDWP hDeferedWindows = BeginDeferWindowPos(4);

    int yOffset = CS.YWINDOW_MARGIN;
    hDeferedWindows = DeferWindowPos(hDeferedWindows, labelMessage1, HWND_NOTOPMOST, CS.XWINDOW_MARGIN, yOffset, totalWidth - (CS.XWINDOW_MARGIN * 2), CD.YLABEL, SWP_NOZORDER);
    yOffset += CD.YLABEL + CS.YRELATED_MARGIN;
    hDeferedWindows = DeferWindowPos(hDeferedWindows, labelMessage2, HWND_NOTOPMOST, CS.XWINDOW_MARGIN, yOffset, totalWidth - (CS.XWINDOW_MARGIN * 2), CD.YLABEL, SWP_NOZORDER);
    yOffset += CD.YLABEL + CS.YRELATED_MARGIN;
    hDeferedWindows = DeferWindowPos(hDeferedWindows, textYourName, HWND_NOTOPMOST, CS.XWINDOW_MARGIN, yOffset, totalWidth - (CS.XWINDOW_MARGIN * 2), CD.YTEXTBOX_ONE_LINE_ALONE, SWP_NOZORDER);
    yOffset += CD.YTEXTBOX_ONE_LINE_ALONE + CS.YRELATED_MARGIN;
    hDeferedWindows = DeferWindowPos(hDeferedWindows, buttonOK, HWND_NOTOPMOST, CS.XWINDOW_MARGIN, yOffset, totalWidth - (CS.XWINDOW_MARGIN * 2), CD.YBUTTON, SWP_NOZORDER);

    EndDeferWindowPos(hDeferedWindows);

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

        case WM_COMMAND:
            
            if(wParam != CtrlID::OK_BUTTON) {
                break;
            }
            // Fall through

        case WM_CLOSE:
            EnableWindow(parentHWnd, TRUE);
            SetFocus(parentHWnd);
            DestroyWindow(hWnd);
            hWnd = NULL;
            break;

    }

    return 0;
}