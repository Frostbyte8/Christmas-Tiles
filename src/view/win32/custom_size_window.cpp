#include "custom_size_window.h"
#include "shared_functions.h"
#include "../../language_table.h"

#ifdef _DPI_AWARE_
#include <shellscalingapi.h>
#endif //_DPI_AWARE_

bool CustomSizeWindow::isRegistered = false;

static const DWORD WINDOW_STYLE = WS_POPUPWINDOW | WS_CAPTION | WS_DLGFRAME | DS_MODALFRAME;
static const DWORD WINDOW_STYLE_EX = WS_EX_DLGMODALFRAME;

__forceinline HWND createLabel(const wchar_t* title, const HWND& parent, const int& ID, const HINSTANCE& hInst) {
    return CreateWindowEx(0, L"Static", title, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, 
                          0, 0, 0, 0, parent, reinterpret_cast<HMENU>(ID), hInst, NULL);
}

__forceinline HWND createButton(const wchar_t* title, const HWND& parent, const DWORD& styles, const int& ID, const HINSTANCE& hInst) {
    return CreateWindowEx(0, L"BUTTON", title, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_PUSHBUTTON | styles, 
                          0, 0, 0, 0, parent, reinterpret_cast<HMENU>(ID), hInst, NULL);
}

namespace CtrlID {
    enum CtrlID {
        ENTER_SIZE = 101,
        XCOORD_LABEL,
        YCOORD_LABEL,
        XCOORD_TEXTBOX,
        YCOORD_TEXTBOX,
        OK_BUTTON,
        CANCEL_BUTTON,
    };
}

//-----------------------------------------------------------------------------
// createWindow - Creates the about Window
//-----------------------------------------------------------------------------

bool CustomSizeWindow::createWindow(const HINSTANCE& hInst, const HWND& parent) {

    if(hWnd) {
        return true; // Already created.
    }

    parentHWnd = parent;

    hWnd = CreateWindowEx(WINDOW_STYLE_EX, L"CustomSizeWindow", GET_LANG_STR(LangID::CUSTOM_SIZE_TITLE),
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

//-----------------------------------------------------------------------------
// onCreate - Processes the WM_CREATE message
//-----------------------------------------------------------------------------

void CustomSizeWindow::onCreate() {

    const HINSTANCE hInst = GetModuleHandle(NULL);

    labelEnterSize = createLabel(GET_LANG_STR(LangID::CUSTOM_SIZE_TEXT), hWnd, CtrlID::ENTER_SIZE, hInst);
    labelCoord[0] = createLabel(L"Width:", hWnd, CtrlID::XCOORD_LABEL, hInst);
    labelCoord[1] = createLabel(L"Height:", hWnd, CtrlID::YCOORD_LABEL, hInst);
    
    textCoord[0] = CreateWindowEx(WS_EX_CLIENTEDGE, L"Edit", L"5", WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_CENTER | ES_NUMBER, 0, 0, 0, 0, hWnd, (HMENU)CtrlID::XCOORD_TEXTBOX, hInst, 0);
    textCoord[1] = CreateWindowEx(WS_EX_CLIENTEDGE, L"Edit", L"5", WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_CENTER | ES_NUMBER, 0, 0, 0, 0, hWnd, (HMENU)CtrlID::YCOORD_TEXTBOX, hInst, 0);

    SendMessage(textCoord[0], EM_SETLIMITTEXT, 2, 0);
    SendMessage(textCoord[1], EM_SETLIMITTEXT, 2, 0);

    buttonOK = createButton(GET_LANG_STR(LangID::OK_BUTTON_CAPTION), hWnd, BS_DEFPUSHBUTTON, CtrlID::OK_BUTTON, hInst);
    buttonCancel = createButton(GET_LANG_STR(LangID::CANCEL_BUTTON_CAPTION), hWnd, 0, CtrlID::CANCEL_BUTTON, hInst);

    prevMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);

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

void CustomSizeWindow::onDPIChange(const float xDPI, const float yDPI) {
    metrics.initWindowMetrics(xDPI, yDPI);
    HFONT dialogFont = metrics.GetCurrentFont();
    EnumChildWindows(hWnd, reinterpret_cast<WNDENUMPROC>(ChangeControlsFont), (LPARAM)dialogFont);
    if(!windowMoving) {
        moveControls();
    }
}

//------------------------------------------------------------------------------
// moveControls - Move/Resize Controls to their proper positions, and then
// centers the window as this only happens when you change the board size
//------------------------------------------------------------------------------

void CustomSizeWindow::moveControls() {

    const ControlDimensions& CD = metrics.getControlDimensions();
    const ControlSpacing& CS = metrics.getControlSpacing();

    const LONG totalWidth = metrics.calculateStringWidth(GET_LANG_STR(LangID::CUSTOM_SIZE_TEXT)) + (CS.XWINDOW_MARGIN * 2);
    const LONG totalHeight = (CS.YWINDOW_MARGIN * 2) + CD.YLABEL + CD.YTEXTBOX_ONE_LINE_ALONE + CD.YBUTTON + (CS.YRELATED_MARGIN * 2);

    LONG LabelLength    = max(metrics.calculateStringWidth(L"Width: "), metrics.calculateStringWidth(L"Height: "));
    LONG textBoxLength  = metrics.calculateStringWidth(L"00");

    RECT tempRC = {0, 0, textBoxLength, 0};
    AdjustWindowRectEx(&tempRC, WS_VISIBLE, FALSE, WS_EX_CLIENTEDGE);
    textBoxLength += abs(tempRC.right - tempRC.left);

    HDWP hDeferedWindows = BeginDeferWindowPos(7);

    // First line is easy, this is just for the Label at the top and it gets the whole width
    int yOffset = CS.YWINDOW_MARGIN;
    hDeferedWindows = DeferWindowPos(hDeferedWindows, labelEnterSize, HWND_NOTOPMOST, CS.XWINDOW_MARGIN, yOffset, totalWidth - (CS.XWINDOW_MARGIN * 2), CD.YLABEL, SWP_NOZORDER);
    yOffset += CD.YLABEL + CS.YRELATED_MARGIN;
    
    // The second line, we need to calculate how long it will be, and where the center is.
    // TODO [DPI]: the total length could exceed totalWidth, so it should be in that calculation
    int xOffset = (totalWidth / 2) - ( ((LabelLength * 2) + (textBoxLength * 2) + (CS.XLABELASSOC_MARGIN * 2) + (CS.XUNRELATED_MARGIN)) / 2);

    // First Set of Coordinates
    hDeferedWindows = DeferWindowPos(hDeferedWindows, labelCoord[0], HWND_NOTOPMOST, xOffset, yOffset + CS.YLABELSAMELINE_MARGIN, LabelLength, CD.YLABEL, SWP_NOZORDER);
    xOffset += CS.XLABELASSOC_MARGIN + LabelLength;
    hDeferedWindows = DeferWindowPos(hDeferedWindows, textCoord[0], HWND_NOTOPMOST, xOffset, yOffset, textBoxLength, CD.YTEXTBOX_ONE_LINE_ALONE, SWP_NOZORDER);
    xOffset += CS.XUNRELATED_MARGIN + textBoxLength;

    // Second Set of Coodrinates
    hDeferedWindows = DeferWindowPos(hDeferedWindows, labelCoord[1], HWND_NOTOPMOST, xOffset, yOffset + CS.YLABELSAMELINE_MARGIN, LabelLength, CD.YLABEL, SWP_NOZORDER);
    xOffset += CS.XLABELASSOC_MARGIN + LabelLength;
    hDeferedWindows = DeferWindowPos(hDeferedWindows, textCoord[1], HWND_NOTOPMOST, xOffset, yOffset, textBoxLength, CD.YTEXTBOX_ONE_LINE_ALONE, SWP_NOZORDER);

    // Find the postion of the OK and CANCEL buttons
    yOffset += CD.YTEXTBOX_ONE_LINE_ALONE + CS.YRELATED_MARGIN;
    xOffset = (totalWidth / 2) - ( ((CD.XBUTTON * 2) + CS.XRELATED_MARGIN) / 2);
    hDeferedWindows = DeferWindowPos(hDeferedWindows, buttonOK, HWND_NOTOPMOST, xOffset, yOffset, CD.XBUTTON, CD.YBUTTON, SWP_NOZORDER);
    xOffset += CD.XBUTTON + CS.XRELATED_MARGIN;
    hDeferedWindows = DeferWindowPos(hDeferedWindows, buttonCancel, HWND_NOTOPMOST, xOffset, yOffset, CD.XBUTTON, CD.YBUTTON, SWP_NOZORDER);
    
    EndDeferWindowPos(hDeferedWindows);

    //RECT rc = {0, 0, totalWidth, totalHeight};
    tempRC.left = 0;    tempRC.right = totalWidth;
    tempRC.top = 0;     tempRC.bottom = totalHeight;
    
    AdjustWindowRectEx(&tempRC, WINDOW_STYLE, FALSE, WINDOW_STYLE_EX);
    CenterWindow(hWnd, tempRC, prevMonitor);
}

//------------------------------------------------------------------------------
// registerSelf - Registers the Window class.
//------------------------------------------------------------------------------

bool CustomSizeWindow::registerSelf(const HINSTANCE& hInst) {
    
    WNDCLASSEX wc;

    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = CustomSizeWindow::WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInst;
    wc.hIcon         = NULL;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = L"CustomSizeWindow";
    wc.hIconSm       = NULL;

    if(!RegisterClassEx(&wc)) {
        MessageBox(NULL, L"Error registering window class.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// onWindowMoved - Adjust the window, if needed after is has been moved.
//-----------------------------------------------------------------------------

void CustomSizeWindow::onWindowMoved() {
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

LRESULT CustomSizeWindow::windowProc(const UINT& msg, const WPARAM wParam, const LPARAM lParam) {
    bool didCancel = false;
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

            if(wParam == CtrlID::OK_BUTTON) {
                saveData();
            }
            else if (wParam != CtrlID::CANCEL_BUTTON) {
                return DefWindowProc(hWnd, msg, wParam, lParam);
            }
            else {
                didCancel = true;
            }


        case WM_CLOSE:
            SendMessage(parentHWnd, UWM_DIALOG_CLOSED, 0, 0);
            if(!didCancel) {
                SendMessage(parentHWnd, UWM_CUSTOM_SIZE_ENTERED, 0, 0);
            }
            EnableWindow(parentHWnd, TRUE);
            SetFocus(parentHWnd);
            DestroyWindow(hWnd);
            hWnd = NULL;
            break;
#ifdef _DPI_AWARE_
        case WM_DPICHANGED:
            onDPIChange(LOWORD(wParam), HIWORD(wParam));
            break;
#endif // _DPI_AWARE_
    }

    return 0;
}

void CustomSizeWindow::saveData() {
    wchar_t buffer[4] = {0};
    GetWindowText(textCoord[0], buffer, 4);
    newWidth = static_cast<uint8_t>(wcstol(buffer, NULL, 10));
    GetWindowText(textCoord[1], buffer, 4);
    newHeight = static_cast<uint8_t>(wcstol(buffer, NULL, 10));
}