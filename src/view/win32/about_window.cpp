#include "about_window.h"
#include "../../resources/resource.h"
bool AboutWindow::isRegistered = false;

static const DWORD WINDOW_STYLE = WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_THICKFRAME | WS_MINIMIZEBOX);
static const DWORD WINDOW_STYLE_EX = WS_EX_OVERLAPPEDWINDOW;

//-----------------------------------------------------------------------------
// createWindow - Creates the about Window
//-----------------------------------------------------------------------------

// TODO: X,Y,W,H, Style Flags

bool AboutWindow::createWindow(const HINSTANCE& hInst, const HWND& parent) {

    if(hWnd) {
        return true; // Already created.
    }

    parentHWnd = parent;

    hWnd = CreateWindowEx(WINDOW_STYLE_EX, L"AboutWindow", L"",
        WINDOW_STYLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 320, 320,
        parent, NULL, hInst, this);

    if(hWnd == NULL) {
        MessageBox(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return false;
    }

    EnableWindow(parent, FALSE);

    if(!iconHandle) {
        iconHandle = LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON, 32, 32, 0);
        //iconHandle = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_APPICON));
    }
    appIcon = CreateWindow(L"Static", L"", WS_CHILD | WS_VISIBLE | SS_ICON | SS_REALSIZEIMAGE, 0, 0, 32, 32, hWnd, 0, hInst, 0);
    SendMessage(appIcon, STM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)iconHandle);


    // TODO: Center window, monitor info, etc
    ShowWindow(hWnd, SW_NORMAL);
    UpdateWindow(hWnd);

    //CreateWindow(L"Button", L"OK", WS_TABSTOP | WS_CHILD | WS_VISIBLE, 0, 48, 128, 32, hWnd, (HMENU)101, hInst, NULL);

    return true;
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

//-----------------------------------------------------------------------------
// windowProc - Standard window procedure for a window
//-----------------------------------------------------------------------------

LRESULT AboutWindow::windowProc(const UINT& msg, const WPARAM wParam, const LPARAM lParam) {

    switch(msg) {
        
        default: return DefWindowProc(hWnd, msg, wParam, lParam);
        case WM_CLOSE:
            EnableWindow(parentHWnd, TRUE);
            SetFocus(parentHWnd);
            DestroyWindow(hWnd);
            hWnd = NULL;
            break;

    }

    return 0;
}