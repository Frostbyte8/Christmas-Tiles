#include "main_window_view.h"
#include "subclassed_controls.h"
#include <math.h>
#include <sstream>

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

    window = CreateWindowEx(WS_EX_CLIENTEDGE,
        L"XmasTilesMainWindow",
        L"",
        WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_THICKFRAME),
        CW_USEDEFAULT, CW_USEDEFAULT, 200, 200,
        NULL, NULL, hInstance, this);

    if(window == NULL) {
        MessageBox(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return false;
    }

    // TODO: Bind presenter to view
    // Some of this needs to happen after createControls()
    gamePresenter = new GamePresenter(this);
    gamePresenter->changeBoardSize(3, 3, 16);
    gamePresenter->tryNewGame();

    tileset = (HBITMAP)LoadImage(NULL, L"gfx.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);  
    if(!tileset) {
        MessageBox(NULL, L"Could not load gfx.bmp", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return false;
    }

    createControls();

    gameBoardPanel.setTileset(tileset);
    gameBoardPanel.gamePresenter = gamePresenter;

    // Get Monitor Info
    prevMonitor = MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);
    updateLabels(); // TODO: Presenter calls this, though it has to happen 
    createMenubar();
    moveControls();
    ShowWindow(window, SW_NORMAL);
    UpdateWindow(window);

    SetTimer(window, TimerIDs::UPDATE_TIMER, 100, NULL);

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
    WNDPROC* oldProc = NULL;
    switch(msg)
    {
        default:
            return DefWindowProc(window, msg, wParam, lParam);

        case BP_CLICKED:
            return onGameBoardClick(wParam, lParam);

        case WM_COMMAND:
            if(HIWORD(wParam) == 0) {
                onSelectMenuItem(LOWORD(wParam));
            }
            break;

        //case WM_PAINT:          return onPaint();
        case WM_TIMER:          return onTimer(wParam);
        case WM_EXITSIZEMOVE:   return onWindowMoved();

        case WM_CLOSE: 
            oldProc = reinterpret_cast<WNDPROC *>(GetWindowLongPtr(controls[ControlIDs::LBL_TIME], GWLP_USERDATA));
            SetWindowLongPtr(controls[ControlIDs::LBL_TIME], GWLP_WNDPROC, (LONG_PTR)oldProc);
            DestroyWindow(window);
            break;
        case WM_DESTROY: PostQuitMessage(0);
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

    WNDPROC* oldProc = reinterpret_cast<WNDPROC*>(SetWindowLongPtr(controls[ControlIDs::LBL_TIME], GWLP_WNDPROC, (LONG_PTR)doubleBufferedLabelProc)); 
    SetWindowLongPtr(controls[ControlIDs::LBL_TIME], GWLP_USERDATA, reinterpret_cast<LONG_PTR>(oldProc));

    HFONT dialogFont = wm.GetCurrentFont();
    EnumChildWindows(window, reinterpret_cast<WNDENUMPROC>(SetProperFont), (LPARAM)dialogFont);

    gameBoardPanel.registerSelf(GetModuleHandle(NULL));
    controls[ControlIDs::VIEW_GAMEBOARD] = CreateWindowEx(0, GameBoardPanel::getWndClassName(), L"", WS_CHILD | WS_VISIBLE,
                                                          0, 0, 32, 32,
                                                          window, (HMENU)MAKE_ID(ControlIDs::VIEW_GAMEBOARD), GetModuleHandle(NULL), &gameBoardPanel);



    return true;
}

//-----------------------------------------------------------------------------
// createMenubar - Create the Window's menubar, and populate it.
//-----------------------------------------------------------------------------

void MainWindowView::createMenubar() {
    HMENU menuBar = CreateMenu();
    HMENU fileMenu = CreateMenu();
    HMENU optionsMenu = CreateMenu();
    HMENU helpMenu = CreateMenu();

    AppendMenu(fileMenu, MF_STRING, MenuIDs::NEW_GAME, L"&New Game");
    AppendMenu(fileMenu, MF_STRING, MenuIDs::PAUSE_GAME, L"&Pause Game");
    AppendMenu(fileMenu, MF_SEPARATOR, 0, 0);
    AppendMenu(fileMenu, MF_STRING, MenuIDs::HIGHSCORES, L"&Highscores");
    AppendMenu(fileMenu, MF_SEPARATOR, 0, 0);
    AppendMenu(fileMenu, MF_STRING, MenuIDs::EXIT, L"E&xit");

    AppendMenu(optionsMenu, MF_STRING, MenuIDs::TILESET, L"&Tileset...");
    AppendMenu(optionsMenu, MF_SEPARATOR, 0, 0);
    AppendMenu(optionsMenu, MF_STRING | MF_CHECKED, MenuIDs::BOARD_3X3, L"3x3");
    AppendMenu(optionsMenu, MF_STRING, MenuIDs::BOARD_4X4, L"4x4");
    AppendMenu(optionsMenu, MF_STRING, MenuIDs::BOARD_5X5, L"5x5");
    AppendMenu(optionsMenu, MF_STRING, MenuIDs::BOARD_5X9, L"5x9");
    AppendMenu(optionsMenu, MF_STRING, MenuIDs::BOARD_10x10, L"10x10");
    AppendMenu(optionsMenu, MF_STRING, MenuIDs::BOARD_CUSTOM, L"&Custom Size...");

    AppendMenu(helpMenu, MF_STRING, MenuIDs::HELP_FILE, L"&Help...");
    AppendMenu(helpMenu, MF_SEPARATOR, 0, 0);
    AppendMenu(helpMenu, MF_STRING, MenuIDs::ABOUT, L"&About Holiday Tiles...");

    AppendMenu(menuBar, MF_STRING | MF_POPUP, (UINT_PTR)fileMenu, L"&File");
    AppendMenu(menuBar, MF_STRING | MF_POPUP, (UINT_PTR)optionsMenu, L"&Options");
    AppendMenu(menuBar, MF_STRING | MF_POPUP, (UINT_PTR)helpMenu, L"&Help");

    SetMenu(window, menuBar);
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

    const WORD gameXPos = static_cast<WORD>(widestLabel + grpXSpacing);

    MoveWindow(controls[ControlIDs::VIEW_GAMEBOARD], gameXPos, 0, BOARD_WIDTH, BOARD_HEIGHT, TRUE);

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
    AdjustWindowRectEx(&rc, style, TRUE, exStyle);
    // TODO: Get X/Y Pos
    MoveWindow(window, 0, 0, rc.right - rc.left, rc.bottom - rc.top, TRUE);
    centerWindow();

    return;
}

//-----------------------------------------------------------------------------
// onWindowMoved - Adjust the window, if needed after is has been moved.
//-----------------------------------------------------------------------------

LRESULT MainWindowView::onWindowMoved() {
    HMONITOR currentMonitor = MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);

    if(currentMonitor != prevMonitor) {
        prevMonitor = currentMonitor;
        centerWindow();
    }

    return 0;
}

//-----------------------------------------------------------------------------
// onGameBoardClick - sent when the GameBoard has been clicked
//-----------------------------------------------------------------------------

LRESULT MainWindowView::onGameBoardClick(const WPARAM& wParam, const LPARAM& lParam) {

    WORD xPos = static_cast<WORD>(wParam);
    WORD yPos = static_cast<WORD>(lParam);

    // Make sure the cursor is within the bounds of the gameboard

    if(xPos < 0 || yPos < 0) {
        return 0;
    }

    // Convert x/y to Index

    xPos = xPos / TILE_SIZE;
    yPos /= TILE_SIZE;

    // DEBUG: For test
    const uint8_t& width = gamePresenter->getWidth(); 
    const uint8_t& height = gamePresenter->getHeight();

    if(xPos > width - 1) {
        return 0;
    }

    if(yPos > height - 1) {
        return 0;
    }

    yPos = (yPos * width) + xPos;

    //const GameTile* tiles = gamePresenter->getTiles();

    if(gamePresenter->tryFlipTile(yPos)) {
        InvalidateRect(window, NULL, FALSE);
        if(gamePresenter->bothFlipped()) {
            SetTimer(window, TimerIDs::UNFLIP_TIMER, 1000, NULL);
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
// onSelectMenuItem - Process when a menu item is selected, with one exception,
// the Pause Button is also processed here.
// @param WORD containing the ID of the menu item selected.
//-----------------------------------------------------------------------------

void MainWindowView::onSelectMenuItem(const WORD& itemID) {

    const uint8_t boardSizes[][2] = {{3, 3}, {4, 4}, {5, 5}, {5, 9}, {10, 10}};
    WORD indexOffset;

    switch(itemID) {
        // File Menu
        case MenuIDs::NEW_GAME:
            if(gamePresenter->tryNewGame()) {
                updateLabels();
                InvalidateRect(window, NULL, FALSE);
            }
            break;
        // Options Menu
        case MenuIDs::BOARD_3X3:
        case MenuIDs::BOARD_4X4:
        case MenuIDs::BOARD_5X5:
        case MenuIDs::BOARD_5X9:
        case MenuIDs::BOARD_10x10:
            indexOffset = itemID - MenuIDs::BOARD_3X3;
            // TODO: Changing the board size should automatically start a new game.
            gamePresenter->changeBoardSize(boardSizes[indexOffset][0], boardSizes[indexOffset][1], 16);
            gamePresenter->tryNewGame(true);
            updateLabels();
            InvalidateRect(window, NULL, FALSE);
            break;
    }

}

//-----------------------------------------------------------------------------
// onTimer - Sent when a timer has elapsed its time.
// @param UINT containing the ID of the timer
//-----------------------------------------------------------------------------

LRESULT MainWindowView::onTimer(const UINT& timerID) {

    if(timerID == TimerIDs::UNFLIP_TIMER) {
        if(gamePresenter->bothFlipped()) {
            gamePresenter->unflip();
            InvalidateRect(window, NULL, FALSE);
        }
        KillTimer(window, timerID);
    }
    else if(timerID == TimerIDs::UPDATE_TIMER) {

        if(gamePresenter->getGameState() == GameState::PLAYING) {
            updateLabels();
        }

    }
    return 0;
}

//-----------------------------------------------------------------------------
// updateLabels - Updates the Score/Points/Time labels
//-----------------------------------------------------------------------------

void MainWindowView::updateLabels() {

    uint32_t seconds = (gamePresenter->getElapsedTime()) / 1000;
    uint32_t minutes = seconds / 60;
    seconds = seconds - (minutes * 60);

    wchar_t timeStr[32] = {0};
    wsprintf(timeStr, L"%02d:%02d", minutes, seconds);
    SetWindowTextW(controls[ControlIDs::LBL_TIME], timeStr);
    InvalidateRect(controls[ControlIDs::LBL_TIME], NULL, FALSE);

}

//=============================================================================
// Public Interface Functions
//=============================================================================

void MainWindowView::gameWon() {
    MessageBox(NULL, L"WIN", L"WIN", MB_OK);
}

int MainWindowView::askQuestion(const std::string& message, const std::string& title, const int& mbType) {
    // TODO: UTF-8 std::string to wchar.
    return MessageBoxA(window, message.c_str(), title.c_str(), mbType);
}
