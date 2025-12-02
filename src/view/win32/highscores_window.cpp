#include "highscores_window.h"
#include "../../language_table.h"
#include "shared_functions.h"

#include <shellscalingapi.h>

#include <assert.h>

bool HighscoresWindow::isRegistered = false;

static const DWORD WINDOW_STYLE = WS_POPUPWINDOW | WS_CAPTION | WS_DLGFRAME | DS_MODALFRAME;
static const DWORD WINDOW_STYLE_EX = WS_EX_DLGMODALFRAME;

__forceinline HWND createLabel(const wchar_t* title, const DWORD& align, const HWND& parent, const int& ID, const HINSTANCE& hInst) {
    return CreateWindowEx(0, L"Static", title, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | align, 
                          0, 0, 0, 0, parent, reinterpret_cast<HMENU>(ID), hInst, NULL);
}

__forceinline HWND createButton(const wchar_t* title, const HWND& parent, const int& ID, const HINSTANCE& hInst) {
    return CreateWindowEx(0, L"BUTTON", title, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_DEFPUSHBUTTON, 
                          0, 0, 0, 0, parent, reinterpret_cast<HMENU>(ID), hInst, NULL);
}

namespace CtrlID {
    enum CtrlID {
        OK_BUTTON = 101,
    };
}

//-----------------------------------------------------------------------------
// createWindow - Creates the about Window
//-----------------------------------------------------------------------------

bool HighscoresWindow::createWindow(const HINSTANCE& hInst, const HWND& parent, const ScoreTable& inScoreTable) {

    scoreTable = &inScoreTable;

    if(hWnd) {
        return true; // Already created.
    }

    parentHWnd = parent;

    hWnd = CreateWindowEx(WINDOW_STYLE_EX, L"HighscoresWindow", GET_LANG_STR(LangID::HIGHSCORES_TITLE),
        WINDOW_STYLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 0, 0,
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

bool HighscoresWindow::registerSelf(const HINSTANCE& hInst) {
    
    WNDCLASSEX wc;

    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = HighscoresWindow::WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInst;
    wc.hIcon         = NULL;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = L"HighscoresWindow";
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

void HighscoresWindow::onCreate() {

    assert(scoreTable != NULL);

    const HINSTANCE hInst = GetModuleHandle(NULL);

    const std::vector<ScoreT>& scores = scoreTable->getScores();

    labelCaption = createLabel(GET_LANG_STR(LangID::HIGHSCORES_TEXT), SS_CENTER, hWnd, 100, hInst);

    labelHeader[0] = createLabel(GET_LANG_STR(LangID::NAME_HEADER), SS_CENTER, hWnd, 100, hInst);
    labelHeader[1] = createLabel(GET_LANG_STR(LangID::SCORE_BOX_CAPTION), SS_CENTER, hWnd, 100, hInst);
    labelHeader[2] = createLabel(GET_LANG_STR(LangID::DATE_HEADER), SS_CENTER, hWnd, 100, hInst);

    for(size_t i = 0; i < scores.size(); ++i) {
        labelName[i]    = createLabel(scores[i].name, SS_LEFT, hWnd, 100, hInst);

        wchar_t score[16] = {0};
        swprintf_s(score, 10, L"%d", scores[i].score);
        labelScore[i] = createLabel(score, SS_CENTER, hWnd, 100, hInst);

        wchar_t date[16] = {0};
        swprintf_s(date, 11, L"%d/%02d/%02d", scores[i].year, scores[i].month, scores[i].day);
        labelDate[i] = createLabel(date, SS_CENTER, hWnd, 100, hInst);
    }


    buttonOK = createButton(GET_LANG_STR(LangID::OK_BUTTON_CAPTION), hWnd, CtrlID::OK_BUTTON, hInst);
    SendMessage(hWnd, DM_SETDEFID, CtrlID::OK_BUTTON, 0);

    prevMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);

    UINT xDPI;
    UINT yDPI;
    GetDpiForMonitor(prevMonitor, MONITOR_DPI_TYPE::MDT_DEFAULT, &xDPI, &yDPI);
    onDPIChange(static_cast<float>(xDPI), static_cast<float>(yDPI));

    moveControls();
    windowMoving = false;
    ShowWindow(hWnd, SW_NORMAL);
    UpdateWindow(hWnd);

}

//------------------------------------------------------------------------------
// onDPIChange - Processes the WM_DPICHANGED messages
//------------------------------------------------------------------------------

void HighscoresWindow::onDPIChange(const float xDPI, const float yDPI) {
    metrics.initWindowMetrics(xDPI, yDPI);
    HFONT dialogFont = metrics.GetCurrentFont();
    EnumChildWindows(hWnd, reinterpret_cast<WNDENUMPROC>(ChangeControlsFont), (LPARAM)dialogFont);
    if(!windowMoving) {
        moveControls();
    }
}

LONG HighscoresWindow::findWidestName() {

    LONG retVal = 0;
    const std::vector<ScoreT>& scores = scoreTable->getScores();
    wchar_t name[ScoreTableConstants::MAX_NAME_LENGTH+1] = {0};

    for(size_t i = 0; i < scores.size(); ++i) {
        GetWindowText(labelName[i], name, ScoreTableConstants::MAX_NAME_LENGTH+1);
        retVal = max(metrics.calculateStringWidth(name), retVal);
    }

    // Were going to make an assumption that the name label caption can never be bigger than an actually name,
    // surely there isn't a language like that, right?

    GetWindowText(labelHeader[0], name, ScoreTableConstants::MAX_NAME_LENGTH);
    retVal = max(metrics.calculateStringWidth(name), retVal);

    return retVal;
}

void HighscoresWindow::moveControls() {
    
    const ControlDimensions& CD = metrics.getControlDimensions();
    const ControlSpacing& CS = metrics.getControlSpacing();

    const LONG NAME_WIDTH   = findWidestName(); //metrics.calculateStringWidth(L"WWWWWWWWWWWWWWWWWWWWWWWWW");
    const LONG SCORE_WIDTH  = metrics.calculateStringWidth(L"000000000");
    const LONG DATE_WIDTH   = metrics.calculateStringWidth(L"1900/01/01");

    const LONG SCORE_XOFFSET    = CS.XWINDOW_MARGIN + CS.XRELATED_MARGIN + NAME_WIDTH;
    const LONG DATE_XOFFSET     = CS.XRELATED_MARGIN + SCORE_XOFFSET + SCORE_WIDTH;
    
    const LONG totalWidth = (CS.XWINDOW_MARGIN * 2) + (CS.XRELATED_MARGIN * 2) + NAME_WIDTH + SCORE_WIDTH + DATE_WIDTH;
    const LONG totalHeight = (CS.YWINDOW_MARGIN * 2) + (CD.YLABEL * 12) + CD.YBUTTON + (CS.YRELATED_MARGIN * 12);

    HDWP hDeferedWindows = BeginDeferWindowPos(5 + (3 * ScoreTableConstants::MAX_SCORES));

    int yOffset = CS.YWINDOW_MARGIN;

    hDeferedWindows = DeferWindowPos(hDeferedWindows, labelCaption, HWND_NOTOPMOST, CS.XWINDOW_MARGIN, yOffset, totalWidth - (CS.XWINDOW_MARGIN * 2), CD.YLABEL, SWP_NOZORDER);
    yOffset += CD.YLABEL + CS.YRELATED_MARGIN;
    hDeferedWindows = DeferWindowPos(hDeferedWindows, labelHeader[0], HWND_NOTOPMOST, CS.XWINDOW_MARGIN, yOffset, NAME_WIDTH, CD.YLABEL, SWP_NOZORDER);
    hDeferedWindows = DeferWindowPos(hDeferedWindows, labelHeader[1], HWND_NOTOPMOST, SCORE_XOFFSET , yOffset, SCORE_WIDTH, CD.YLABEL, SWP_NOZORDER);
    hDeferedWindows = DeferWindowPos(hDeferedWindows, labelHeader[2], HWND_NOTOPMOST, DATE_XOFFSET, yOffset, DATE_WIDTH, CD.YLABEL, SWP_NOZORDER);
    yOffset += CD.YLABEL + CS.YRELATED_MARGIN;

    for(int i = 0; i < ScoreTableConstants::MAX_SCORES; ++i) {

        hDeferedWindows = DeferWindowPos(hDeferedWindows, labelName[i], HWND_NOTOPMOST, CS.XWINDOW_MARGIN, yOffset, NAME_WIDTH, CD.YLABEL, SWP_NOZORDER);
        hDeferedWindows = DeferWindowPos(hDeferedWindows, labelScore[i], HWND_NOTOPMOST, SCORE_XOFFSET , yOffset, SCORE_WIDTH, CD.YLABEL, SWP_NOZORDER);
        hDeferedWindows = DeferWindowPos(hDeferedWindows, labelDate[i], HWND_NOTOPMOST, DATE_XOFFSET, yOffset, DATE_WIDTH, CD.YLABEL, SWP_NOZORDER);
        yOffset += CD.YLABEL + CS.YRELATED_MARGIN;

    }

    hDeferedWindows = DeferWindowPos(hDeferedWindows, buttonOK, HWND_NOTOPMOST, (totalWidth / 2) - (CD.XBUTTON / 2), yOffset, CD.XBUTTON, CD.YBUTTON, SWP_NOZORDER);

    EndDeferWindowPos(hDeferedWindows);

    RECT rc = {0, 0, totalWidth, totalHeight};
    AdjustWindowRectEx(&rc, WINDOW_STYLE, FALSE, WINDOW_STYLE_EX);

    CenterWindow(hWnd, rc, prevMonitor);
    
}

//-----------------------------------------------------------------------------
// onWindowMoved - Adjust the window, if needed after is has been moved.
//-----------------------------------------------------------------------------

void HighscoresWindow::onWindowMoved() {
    HMONITOR currentMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);

    if(currentMonitor != prevMonitor) {
        prevMonitor = currentMonitor;
        moveControls(); // This will also center it
        RedrawWindow(hWnd, 0, 0, RDW_INVALIDATE);
    }

}

//-----------------------------------------------------------------------------
// windowProc - Standard window procedure for a window
//-----------------------------------------------------------------------------

LRESULT HighscoresWindow::windowProc(const UINT& msg, const WPARAM wParam, const LPARAM lParam) {

    switch(msg) {
        
        default: return DefWindowProc(hWnd, msg, wParam, lParam);
        case WM_EXITSIZEMOVE: 
            windowMoving = false;
            onWindowMoved();
            break;
        case WM_CREATE:
            onCreate();
            break;
        case WM_ENTERSIZEMOVE:
            windowMoving = true;
            break;
        case DM_GETDEFID:
            return MAKEWPARAM(CtrlID::OK_BUTTON, DC_HASDEFID);

        case WM_COMMAND:
            
            if(wParam != CtrlID::OK_BUTTON) {
                return DefWindowProc(hWnd, msg, wParam, lParam);
            }
            // Fall through

        case WM_CLOSE:
            SendMessage(parentHWnd, UWM_DIALOG_CLOSED, 0, 0);
            EnableWindow(parentHWnd, TRUE);
            SetFocus(parentHWnd);
            DestroyWindow(hWnd);
            hWnd = NULL;
            break;
        case WM_DPICHANGED:
            onDPIChange(LOWORD(wParam), HIWORD(wParam));
            break;
    }

    return 0;
}