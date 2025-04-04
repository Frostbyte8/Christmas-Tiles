#include "main_window_view.h"
#include <math.h>

__forceinline HWND createGroupBox(LPCWSTR windowName, int x, int y, int height, int width, HWND parent, int ID) {
    return CreateWindowEx(0, L"Button", windowName, WS_VISIBLE | WS_CHILD | BS_GROUPBOX, x, y, width, height, parent, (HMENU)ID, GetModuleHandle(NULL), NULL);
}

__forceinline HWND createLabel(LPCWSTR windowName, HWND parent, int ID) {
    return CreateWindowEx(0, L"STATIC", windowName, WS_VISIBLE | WS_CHILD | SS_CENTER, 0, 0, 0, 0, parent, (HMENU)ID, GetModuleHandle(NULL), NULL);
}

#define MAKE_ID(X) (X + 101)

// TODO: This will be obtained from the sprite sheet
const int TILE_SIZE    = 32;

//-----------------------------------------------------------------------------
// registerSelf - Registers the Window class.
//-----------------------------------------------------------------------------

bool MainWindowView::registerSelf(HINSTANCE hInstance) {
    
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

//-----------------------------------------------------------------------------
// createWindow - Creates the main Window.
//-----------------------------------------------------------------------------

bool MainWindowView::createWindow(HINSTANCE hInstance) {

    if(window) {
        return true; // Already created.
    }

    gameBG = CreateSolidBrush(RGB(0, 0, 0));

    window = CreateWindowEx(WS_EX_CLIENTEDGE,
        L"XmasTilesMainWindow",
        L"",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 200, 200,
        NULL, NULL, hInstance, this);

    if(window == NULL) {
        MessageBox(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return false;
    }

    // TODO: Bind presenter to view
    gamePresenter = new GamePresenter(this);
    gamePresenter->changeBoardSize(3, 3, 16);
    gamePresenter->tryNewGame();

    // Load resources
    tileset = (HBITMAP)LoadImage(NULL, L"gfx.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);  

    if(!tileset) {
        MessageBox(NULL, L"Could not load gfx.bmp", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return false;
    }

    createControls();


    // Get Monitor Info
    prevMonitor = MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);

    moveControls();
    ShowWindow(window, SW_NORMAL);
    UpdateWindow(window);

    return true;
}


//-----------------------------------------------------------------------------
// doLoop - Standard run of the mill message loop
//-----------------------------------------------------------------------------

UINT MainWindowView::doLoop() {
    
    MSG msg;
    
    while(GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

//-----------------------------------------------------------------------------
// windowProc - Standard window procedure for a window
//-----------------------------------------------------------------------------

LRESULT MainWindowView::windowProc(const UINT& msg, const WPARAM wParam, const LPARAM lParam) {
    switch(msg)
    {
        default:
            return DefWindowProc(window, msg, wParam, lParam);

        case WM_EXITSIZEMOVE:
            onWindowMoved();            
            break;

        case WM_LBUTTONDOWN:
            onClick(wParam, lParam);
            break;

        case WM_PAINT:
            onPaint();
            break;

        case WM_CLOSE:
            DestroyWindow(window);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

    }
    return 0;
}

//=============================================================================
// Private Functions
//=============================================================================

//-----------------------------------------------------------------------------
// centerWindow - Centers the window on the monitor it was said to have last
// been on.
//-----------------------------------------------------------------------------

void MainWindowView::centerWindow() {

    MONITORINFO monitorInfo = {0};
    monitorInfo.cbSize = sizeof(monitorInfo);
    GetMonitorInfoW(prevMonitor, &monitorInfo);

    RECT windowRC;
    GetWindowRect(window, &windowRC);

    const int winWidth  = windowRC.right - windowRC.left; 
    const int winHeight = windowRC.bottom - windowRC.top;
    const int workXCenter = abs(monitorInfo.rcWork.left - monitorInfo.rcWork.right) / 2;
    const int workYCenter = abs(monitorInfo.rcWork.top - monitorInfo.rcWork.bottom) / 2;

    SetWindowPos(window, NULL,
                 monitorInfo.rcWork.right - (workXCenter + (winWidth / 2)), 
                 monitorInfo.rcWork.bottom - (workYCenter + (winHeight / 2)),
                 winWidth, winHeight, SWP_NOREDRAW);

}

//-----------------------------------------------------------------------------
// createControls - Create the controls used by this window
//-----------------------------------------------------------------------------

bool MainWindowView::createControls() {
    
    const size_t NUM_STRS = 3;
    wchar_t* captions[NUM_STRS] = { L"Score", L"Points", L"Time" };


    for(size_t i = 0; i < NUM_STRS; ++i) {
        const int CTRLID = ControlIDs::GRP_SCORE + i; 
        const int LBLID = ControlIDs::LBL_SCORE + i;
        controls[CTRLID] = createGroupBox(captions[i], 0, 0, 200, 200, window, MAKE_ID(CTRLID));
        controls[LBLID] = createLabel(L"0000000000", window, MAKE_ID(LBLID));
    }

    controls[ControlIDs::BTN_PAUSE] = CreateWindowEx(0, L"Button", L"Pause", WS_CHILD | WS_VISIBLE,
                                                     0, 0, 200, 200,
                                                     window, (HMENU)MAKE_ID(ControlIDs::BTN_PAUSE), GetModuleHandle(NULL), NULL);  

    HFONT dialogFont = wm.GetCurrentFont();
    EnumChildWindows(window, reinterpret_cast<WNDENUMPROC>(SetProperFont), (LPARAM)dialogFont);

    return true;
}

//-----------------------------------------------------------------------------
// moveControls - Move the controls to the correct position
//-----------------------------------------------------------------------------

void MainWindowView::moveControls() {
    

    // find the widest Label. Likely, that will be score/time/points label.

    const WindowMetrics::ControlSpacing& CSPC       = wm.GetControlSpacing();
    const WindowMetrics::ControlDimensions& CDIM    = wm.GetControlDimensions();

    const size_t NUM_STRS = 6;
    wchar_t* captions[NUM_STRS] = { L"Score", L"Points", L"Time", L"Pause", L"Resume", L"0000000000" };

    LONG widestLabel = 0;

    for(size_t i = 0; i < NUM_STRS; ++i) {
        widestLabel = max(widestLabel, wm.CalculateStringWidth(captions[i]));
    }

    // Next we find out how much height a groupbox needs, and any left over
    // space will be taken up by the button

    LONG grpHeight = CDIM.YLABEL + CSPC.YFIRST_GROUPBOX_MARGIN + CSPC.YLAST_GROUPBOX_MARGIN;
    LONG grpXSpacing = CSPC.XGROUPBOX_SPACING * 2;

    // The minimum height of the client area will be either the height of 4 Groupboxes, or
    // the height of the board, whichever is largest. With Groupbox 4 (The Button) taking up the rest
    // of the space remaining.

    const int BOARD_HEIGHT = (grpHeight * 4 > GameConstants::DEF_HEIGHT * TILE_SIZE)
                             ? grpHeight * 4
                             : GameConstants::DEF_HEIGHT * TILE_SIZE;

    const int BOARD_WIDTH   = GameConstants::DEF_WIDTH * TILE_SIZE;

    gameXPos = static_cast<WORD>(widestLabel + grpXSpacing);

    for(int i = 0; i < 3; ++i) {
        const int CTRLID = ControlIDs::GRP_SCORE + i;
        const int LBLID = ControlIDs::LBL_SCORE + i;
        MoveWindow(controls[CTRLID], 0, grpHeight * i, widestLabel + grpXSpacing, grpHeight, TRUE);
        MoveWindow(controls[LBLID], CSPC.XGROUPBOX_SPACING, (grpHeight * i) + CSPC.YFIRST_GROUPBOX_MARGIN, widestLabel, CDIM.YLABEL, TRUE);
    }

    MoveWindow(controls[ControlIDs::BTN_PAUSE], 0, grpHeight * 3, widestLabel + grpXSpacing,
               BOARD_HEIGHT - (grpHeight * 3), TRUE);

    const DWORD style   = static_cast<DWORD>(GetWindowLongPtr(window, GWL_STYLE));
    const DWORD exStyle = static_cast<DWORD>(GetWindowLongPtr(window, GWL_EXSTYLE));
            
    RECT rc = {0, 0, BOARD_WIDTH + widestLabel + grpXSpacing, BOARD_HEIGHT};
    AdjustWindowRectEx(&rc, style, FALSE, exStyle);
    // TODO: Get X/Y Pos
    MoveWindow(window, 0, 0, rc.right - rc.left, rc.bottom - rc.top, TRUE);
    centerWindow();

    return;
}

//-----------------------------------------------------------------------------
// onPaint - Draw the Gameboard
//-----------------------------------------------------------------------------

void MainWindowView::onPaint() {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(window, &ps);
    RECT rc; 

    GetClientRect(window, &rc);
    rc.left = gameXPos;

    FillRect(hdc, &rc, gameBG);

    if(tileset && gamePresenter) {

        const uint8_t& width = gamePresenter->getWidth();
        const uint8_t& height = gamePresenter->getHeight();
        const GameTile* gameTiles = gamePresenter->getTiles();

        HDC hdcSrc = CreateCompatibleDC(hdc);
        HBITMAP oldBMP = (HBITMAP)SelectObject(hdcSrc, tileset);

        for(int k = 0; k < height; ++k) {
            for(int i = 0; i < width; ++i) {

                if(gameTiles[(k * width) + i].matched == 0) {
                    BitBlt(hdc, gameXPos + (i * 32), k * 32, 32, 32, hdcSrc, 512, 0, SRCCOPY);
                }
                else {
                    BitBlt(hdc, gameXPos + (i * 32), k * 32, 32, 32, hdcSrc, 32 * (gameTiles[(k * width) + i].tileType - 1), 0, SRCCOPY);
                }
            }
        }

        SelectObject(hdcSrc, oldBMP);
    }

    EndPaint(window, &ps);
}

//-----------------------------------------------------------------------------
// onWindowMoved - Adjust the window, if needed after is has been moved.
//-----------------------------------------------------------------------------

void MainWindowView::onWindowMoved() {
    HMONITOR currentMonitor = MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);

    if(currentMonitor != prevMonitor) {
        prevMonitor = currentMonitor;
        centerWindow();
    }
}

//-----------------------------------------------------------------------------
// onClick - Sent when the client area of the window has been clicked
//-----------------------------------------------------------------------------

void MainWindowView::onClick(const WPARAM&, const LPARAM& lParam) {

    WORD xPos = LOWORD(lParam);
    WORD yPos = HIWORD(lParam);

    // Make sure the cursor is within the bounds of the gameboard

    if(xPos < gameXPos || yPos < 0) {
        return;
    }

    // Convert x/y to Index

    xPos = (xPos - gameXPos) / TILE_SIZE;
    yPos /= TILE_SIZE;

    OMNICPP_DECL(gamePresenter);
    
    // DEBUG: For test
    const uint8_t& width = gamePresenter->getWidth(); 
    const uint8_t& height = gamePresenter->getHeight();

    if(xPos > width - 1) {
        return;
    }

    if(yPos > height - 1) {
        return;
    }

    yPos = (yPos * width) + xPos;

    const GameTile* tiles = gamePresenter->getTiles();

    if(gamePresenter->tryFlipTile(yPos)) {
        InvalidateRect(window, NULL, FALSE);
    }

}

//-----------------------------------------------------------------------------
// WndProc - A static function of a window proc that allows us to keep data
// for the window encapsulated.
//-----------------------------------------------------------------------------

LRESULT CALLBACK MainWindowView::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    MainWindowView* self;

    if(msg == WM_NCCREATE) {
        // Store a copy of an instance of this window in the USERDATA section
        self = static_cast<MainWindowView*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
        self->window = hWnd;

        SetLastError(0);

        if (!SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self))) {
            const DWORD ERR = GetLastError();
            if(ERR != 0) {
                return FALSE;
            }
        }
    }
    else {
        self = reinterpret_cast<MainWindowView*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    }

    if(self) {
        return self->windowProc(msg, wParam, lParam);
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);

}

//=============================================================================
// Public Interface Functions
//=============================================================================

void MainWindowView::gameWon() {
    MessageBox(NULL, L"WIN", L"WIN", MB_OK);
}
