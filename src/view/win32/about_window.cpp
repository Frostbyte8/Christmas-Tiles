#include "about_window.h"
#include "../../resources/resource.h"
#include "../../language_table.h"
bool AboutWindow::isRegistered = false;

static const DWORD WINDOW_STYLE = WS_POPUPWINDOW | WS_CAPTION | WS_DLGFRAME | DS_MODALFRAME;
static const DWORD WINDOW_STYLE_EX = WS_EX_DLGMODALFRAME;

namespace CtrlID {
    enum CtrlID {
        ICON_APP        = 101,
        TITLE_LABEL,
        VERSION_LABEL,
        SEPERATOR,
        COPYRIGHT_LABEL,
        OK_BUTTON,
    };
}

__forceinline HWND createLabel(const wchar_t* title, const HWND& parent, const int& ID, const HINSTANCE& hInst) {
    return CreateWindowEx(0, L"Static", title, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | SS_CENTER, 
                          0, 0, 0, 0, parent, reinterpret_cast<HMENU>(ID), hInst, NULL);
}

__forceinline HWND createSeperator(const HWND& parent, const int& ID, const HINSTANCE& hInst) {
    return CreateWindowEx(0, L"Static", L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | SS_ETCHEDHORZ, 
                          0, 0, 0, 0, parent, reinterpret_cast<HMENU>(ID), hInst, NULL);
}

__forceinline HWND createButton(const wchar_t* title, const HWND& parent, const int& ID, const HINSTANCE& hInst) {
    return CreateWindowEx(0, L"BUTTON", title, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_PUSHBUTTON, 
                          0, 0, 0, 0, parent, reinterpret_cast<HMENU>(ID), hInst, NULL);
}

//-----------------------------------------------------------------------------
// createWindow - Creates the about Window
//-----------------------------------------------------------------------------

// TODO: X,Y,W,H, Style Flags

bool AboutWindow::createWindow(const HINSTANCE& hInst, const HWND& parent) {

    if(hWnd) {
        return true; // Already created.
    }

    parentHWnd = parent;

    hWnd = CreateWindowEx(WINDOW_STYLE_EX, L"AboutWindow", L"Add Title Here",
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
// moveControls - Move/Resize Controls to their proper positions, and then
// centers the window as this only happens when you change the board size
//------------------------------------------------------------------------------

void AboutWindow::moveControls() {

    const ControlDimensions& CD = metrics.getControlDimensions();
    const ControlSpacing& CS = metrics.getControlSpacing();

    RECT rc = {0, 0, 200, 250};
    AdjustWindowRectEx(&rc, WINDOW_STYLE, FALSE, WINDOW_STYLE_EX);
    MoveWindow(hWnd, 0, 0, rc.right-rc.left, rc.bottom-rc.top, FALSE);

    // TODO: Assuming 200x250 for now

    HDWP hDeferedWindows = BeginDeferWindowPos(5);

    int yOffset = CS.YWINDOW_MARGIN;

    DeferWindowPos(hDeferedWindows, appIcon, HWND_NOTOPMOST, (200/2)-16, yOffset, 32, 32, SWP_NOZORDER);
    yOffset += 32 + CS.YRELATED_MARGIN;
    DeferWindowPos(hDeferedWindows, labelTitle, HWND_NOTOPMOST, CS.XWINDOW_MARGIN, yOffset, 200 - (CS.XWINDOW_MARGIN * 2), CD.YLABEL, SWP_NOZORDER);
    yOffset += CD.YLABEL;
    DeferWindowPos(hDeferedWindows, labelVersion, HWND_NOTOPMOST, CS.XWINDOW_MARGIN, yOffset, 200 - (CS.XWINDOW_MARGIN * 2), CD.YLABEL, SWP_NOZORDER);
    yOffset += CD.YLABEL + CS.YRELATED_MARGIN;
    DeferWindowPos(hDeferedWindows, seperatorBar, HWND_NOTOPMOST, CS.XWINDOW_MARGIN, yOffset, 200 - (CS.XWINDOW_MARGIN * 2), 2, SWP_NOZORDER);
    yOffset += 2 + CS.YRELATED_MARGIN;
    DeferWindowPos(hDeferedWindows, labelCopyright, HWND_NOTOPMOST, CS.XWINDOW_MARGIN, yOffset, 200 - (CS.XWINDOW_MARGIN * 2), CD.YLABEL, SWP_NOZORDER);

    EndDeferWindowPos(hDeferedWindows);
}

//------------------------------------------------------------------------------
// doLoop - Standard run of the mill message loop
//------------------------------------------------------------------------------

UINT AboutWindow::doLoop() {
    
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

bool AboutWindow::registerSelf(const HINSTANCE& hInst) {
    
    WNDCLASSEX wc;

    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = AboutWindow::WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInst;
    wc.hIcon         = NULL;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = L"AboutWindow";
    wc.hIconSm       = NULL;

    if(!RegisterClassEx(&wc)) {
        MessageBox(NULL, L"Error registering window class.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    return true;
}

bool AboutWindow::onCreate() {

    const HINSTANCE hInst = GetModuleHandle(NULL);

    if(!iconHandle) {
        iconHandle = LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON, 32, 32, 0);
        //iconHandle = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_APPICON));
    }

    appIcon = CreateWindow(L"Static", L"", WS_CHILD | WS_VISIBLE | SS_ICON | SS_REALSIZEIMAGE, 0, 0, 32, 32, hWnd, 0, hInst, 0);
    
    SendMessage(appIcon, STM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)iconHandle);

    labelTitle      = createLabel(GET_LANG_STR(LangID::APP_TITLE), hWnd, CtrlID::TITLE_LABEL, hInst);
    
    // TODO: Copyright and Version need a text replacement
    labelVersion    = createLabel(GET_LANG_STR(LangID::VERSION_TEXT), hWnd, CtrlID::VERSION_LABEL, hInst);
    seperatorBar    = createSeperator(hWnd, CtrlID::SEPERATOR, hInst);
    labelCopyright  = createLabel(GET_LANG_STR(LangID::COPYRIGHT_TEXT), hWnd, CtrlID::COPYRIGHT_LABEL, hInst);
    buttonOK        = createButton(GET_LANG_STR(LangID::OK_BUTTON_CAPTION), hWnd, CtrlID::OK_BUTTON, hInst);

    metrics.initWindowMetrics();

    HFONT dialogFont = metrics.GetCurrentFont();
    EnumChildWindows(hWnd, reinterpret_cast<WNDENUMPROC>(SetProperFont), (LPARAM)dialogFont);

    moveControls();

    return true;
}

//-----------------------------------------------------------------------------
// windowProc - Standard window procedure for a window
//-----------------------------------------------------------------------------

LRESULT AboutWindow::windowProc(const UINT& msg, const WPARAM wParam, const LPARAM lParam) {

    switch(msg) {
        
        default: return DefWindowProc(hWnd, msg, wParam, lParam);
        case WM_CREATE:
            onCreate();
            break;
        case WM_CLOSE:
            EnableWindow(parentHWnd, TRUE);
            SetFocus(parentHWnd);
            DestroyWindow(hWnd);
            hWnd = NULL;
            break;

    }

    return 0;
}