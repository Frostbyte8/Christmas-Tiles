#include "main_window.h"
#include "subclassed_controls.h"
#include "../../language_table.h"
#include "../../resources/resource.h"

//==============================================================================
// Namespaces / Enums / Constants
//==============================================================================

static const DWORD WINDOW_STYLE = WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_THICKFRAME);
static const DWORD WINDOW_STYLE_EX = WS_EX_OVERLAPPEDWINDOW | WS_EX_CONTROLPARENT;

namespace MainWindowViewConstants {
    static const int ELAPSED_TIMER_ID   = 1;
    static const int FLIP_TIMER_ID      = 2;
    static const UINT MENU_ENABLED_FLAGS = MF_BYCOMMAND | MF_ENABLED | MF_STRING;
    static const UINT MENU_DISABLED_FLAGS = MF_BYCOMMAND | MF_DISABLED | MF_GRAYED | MF_STRING;
    static const UINT MENU_CHECKED_FLAGS = MF_BYCOMMAND | MF_CHECKED;
    static const UINT MENU_UNCHECKED_FLAGS = MF_BYCOMMAND;
}

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

namespace MenuID {
    enum MenuID {
        NEW_GAME     = 201,
        PAUSE_GAME,
        HIGHSCORES,
        EXIT,
        TILESET,
        CHANGE_LANGUAGE,
        BOARD_3x3,
        BOARD_4x4,
        BOARD_5x5,
        BOARD_5x9,
        BOARD_10x10,
        BOARD_CUSTOM,
        HELP_FILE,
        ABOUT,
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
    return CreateWindowEx(0, L"BUTTON", title, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_PUSHBUTTON, 
                          0, 0, 0, 0, parent, reinterpret_cast<HMENU>(ID), hInst, NULL);
}

__forceinline void updatePoints(const HWND& ctrl, const uint8_t& points) {
    wchar_t pointsStr[4] = {0};
    wsprintf(pointsStr, L"%d", points);
    SetWindowText(ctrl, pointsStr);
}

__forceinline void updateScore(const HWND& ctrl, const uint32_t& score) {
    wchar_t scoreStr[16] = {0};
    wsprintf(scoreStr, L"%09d", score);
    SetWindowText(ctrl, scoreStr);
}

//==============================================================================
// Public Functions
//==============================================================================

//------------------------------------------------------------------------------
// createWindow - Creates the main Window.
//------------------------------------------------------------------------------

bool MainWindowView::createWindow() {

    if(hWnd) {
        return true; // Already created.
    }

    // TODO: This may need WS_CLIPCHILDREN / WS_CLIPSIBLINGS?

    hWnd = CreateWindowEx(WINDOW_STYLE_EX, L"XmasTilesMainWindow", GET_LANG_STR(LangID::APP_TITLE),
        WINDOW_STYLE, CW_USEDEFAULT, CW_USEDEFAULT, 200, 200,
        NULL, NULL, hInstance, this);

    if(hWnd == NULL) {
        MessageBox(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return false;
    }

    ShowWindow(hWnd, SW_NORMAL);
    UpdateWindow(hWnd);

    // TODO: This should be set for the first time when the game starts.
    SetTimer(hWnd, MainWindowViewConstants::ELAPSED_TIMER_ID, 100, 0);

    return true;
}

//------------------------------------------------------------------------------
// doLoop - Standard run of the mill message loop
//------------------------------------------------------------------------------

UINT MainWindowView::doLoop() {
    
    MSG msg;
    
    while(GetMessage(&msg, NULL, 0, 0) > 0) {
        
        // TODO: Cache which window is open instead

        if(aboutWindow.getHandle() && IsDialogMessage(aboutWindow.getHandle(), &msg)) {
        }
        else if (!IsDialogMessage(hWnd, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
 
    }

    return msg.wParam;
}

//------------------------------------------------------------------------------
// registerSelf - Registers the Window class.
//------------------------------------------------------------------------------

bool MainWindowView::registerSelf() {
    
    WNDCLASSEX wc;

    // TODO: DPI
    
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = MainWindowView::WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON, 32, 32, 0);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = L"XmasTilesMainWindow";
    wc.hIconSm       = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON, 16, 16, 0);

    if(!RegisterClassEx(&wc)) {
        MessageBox(NULL, L"Error registering window class.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    return true;
}

//==============================================================================
// Win32 Messages
//==============================================================================

__forceinline void MainWindowView::createMenuBar() {
    menuBar     = CreateMenu();
    fileMenu    = CreateMenu();
    optionsMenu = CreateMenu();
    helpMenu    = CreateMenu();

    // File Menu

    AppendMenu(fileMenu, MF_STRING, MenuID::NEW_GAME, L"&New Game");
    AppendMenu(fileMenu, MF_STRING | MF_DISABLED | MF_GRAYED, MenuID::PAUSE_GAME, L"&Pause");
    AppendMenu(fileMenu, MF_SEPARATOR, 0, 0);
    AppendMenu(fileMenu, MF_STRING, MenuID::HIGHSCORES, L"&Highscores");
    AppendMenu(fileMenu, MF_SEPARATOR, 0, 0);
    AppendMenu(fileMenu, MF_STRING, MenuID::EXIT, L"&Exit");

    // Options Menu
    // TODO: MenuItemInfo may be necessary for radio buttons
    AppendMenu(optionsMenu, MF_STRING, MenuID::TILESET, L"&Change Tileset...");
    AppendMenu(optionsMenu, MF_STRING, MenuID::CHANGE_LANGUAGE, L"&Change Language...");
    AppendMenu(optionsMenu, MF_SEPARATOR, 0, 0);
    AppendMenu(optionsMenu, MF_STRING | MF_CHECKED, MenuID::BOARD_3x3, L"3x3");
    AppendMenu(optionsMenu, MF_STRING, MenuID::BOARD_4x4, L"4x4");
    AppendMenu(optionsMenu, MF_STRING, MenuID::BOARD_5x5, L"5x5");
    AppendMenu(optionsMenu, MF_STRING, MenuID::BOARD_5x9, L"5x9");
    AppendMenu(optionsMenu, MF_STRING, MenuID::BOARD_10x10, L"10x10");
    AppendMenu(optionsMenu, MF_STRING, MenuID::BOARD_CUSTOM, L"&Custom Size...");

    // Help Menu
    AppendMenu(helpMenu, MF_STRING, MenuID::HELP_FILE, L"&Help...");
    AppendMenu(helpMenu, MF_SEPARATOR, 0, 0);
    AppendMenu(helpMenu, MF_STRING, MenuID::ABOUT, L"&About Christmas Tiles...");

    // Now attach each menu to the menu bar
    AppendMenu(menuBar, MF_STRING | MF_POPUP, reinterpret_cast<UINT_PTR>(fileMenu), L"&File");
    AppendMenu(menuBar, MF_STRING | MF_POPUP, reinterpret_cast<UINT_PTR>(optionsMenu), L"&Options");
    AppendMenu(menuBar, MF_STRING | MF_POPUP, reinterpret_cast<UINT_PTR>(helpMenu), L"&Help");

    SetMenu(hWnd, menuBar);
}


//------------------------------------------------------------------------------
// onCreate - Processes the WM_CREATE message
//------------------------------------------------------------------------------

bool MainWindowView::onCreate() {

    createMenuBar();

    static const DWORD DEF_STYLE_FLAGS = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS;
    
    groupStats[0] = createGroupBox(GET_LANG_STR(LangID::SCORE_BOX_CAPTION), hWnd, CtrlID::GROUP_SCORE, hInstance);
    groupStats[1] = createGroupBox(GET_LANG_STR(LangID::POINTS_BOX_CAPTION), hWnd, CtrlID::GROUP_POINTS, hInstance);
    groupStats[2] = createGroupBox(GET_LANG_STR(LangID::TIME_BOX_CAPTION), hWnd, CtrlID::GROUP_TIME, hInstance);

    scoreLabel.createWindow(hInstance, L"000000000", hWnd, reinterpret_cast<HMENU>(CtrlID::LABEL_SCORE));
    pointsLabel.createWindow(hInstance, L"50", hWnd, reinterpret_cast<HMENU>(CtrlID::LABEL_POINTS));
    timeLabel.createWindow(hInstance, L"00:00", hWnd, reinterpret_cast<HMENU>(CtrlID::LABEL_TIME));

    buttonPause = createButton(GET_LANG_STR(LangID::PAUSE_BUTTON_CAPTION), hWnd, CtrlID::BUTTON_PAUSE, hInstance);
    EnableWindow(buttonPause, FALSE);

    // TODO: gamePanel should get the presenter via it's constructor?
    gamePanel.createWindow(hInstance, hWnd, reinterpret_cast<HMENU>(CtrlID::PANEL_GAMEBOARD));

    // TODO: Window Presenter 
    gamePanel.setWindowPresenter(&windowPresenter);
    windowPresenter.tryUpdateGameBoard(3, 3, static_cast<uint8_t>( (gamePanel.getTilesetWidth() / gamePanel.getTilesetHeight()) - 2));
   

    metrics.initWindowMetrics();

    HFONT dialogFont = metrics.GetCurrentFont();
    EnumChildWindows(hWnd, reinterpret_cast<WNDENUMPROC>(SetProperFont), (LPARAM)dialogFont);

    prevMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);


    moveControls();
    return true;

}

//==============================================================================
// Private Functions
//==============================================================================

//------------------------------------------------------------------------------
// centerWindow - Centers the window on the monitor it was said to have last
// been on.
//------------------------------------------------------------------------------

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
// getTallestPoint - Obtain the tallest point of the window.
//------------------------------------------------------------------------------

// TODO: Tile Size, etc.

LONG MainWindowView::getTallestPoint() const {
    const ControlDimensions& CD = metrics.getControlDimensions();
    const ControlSpacing& CS = metrics.getControlSpacing();

    const long minHeight = gamePanel.getTilesetHeight() * 9;
    const long boardheight = gamePanel.getTilesetHeight() * windowPresenter.getGameBoard().getHeight();


    LONG tallestPoint = (CD.YLABEL * 3) + (CS.YFIRST_GROUPBOX_MARGIN * 3) + (CS.YLAST_GROUPBOX_MARGIN * 3) +
                        (CD.YBUTTON);

    tallestPoint = tallestPoint < boardheight ? boardheight : tallestPoint;
    
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
    // TODO: minWidth = GroupBox + 5 tiles
    // TODO: Min Height = the Height of Standard Tile. So that is gamePanel.getTilesetHeight() * 9

    // Resize Window
    // TODO: Get Monitor Window is on?

    int horzTiles = windowPresenter.getGameBoard().getWidth();
    if(horzTiles < 5) {
        horzTiles = 5;
    }

    RECT rc = {0, 0, (gamePanel.getTilesetHeight() * horzTiles) + widestGroupBox, tallestPoint};
    AdjustWindowRectEx(&rc, WINDOW_STYLE, TRUE, WINDOW_STYLE_EX);

    // TODO: This causes flicker as the window is not cenetered
    MoveWindow(hWnd, 0, 0, rc.right-rc.left, rc.bottom-rc.top, TRUE);

    const LONG boxHeight = CD.YLABEL + CS.YFIRST_GROUPBOX_MARGIN + CS.YLAST_GROUPBOX_MARGIN;
    
    // Move Controls into position

    HDWP hDeferedWindows = BeginDeferWindowPos(8);

    hDeferedWindows = DeferWindowPos(hDeferedWindows, groupStats[0], HWND_NOTOPMOST, 0, 0, widestGroupBox, boxHeight, SWP_NOZORDER);
    hDeferedWindows = DeferWindowPos(hDeferedWindows, groupStats[1], HWND_NOTOPMOST, 0, boxHeight, widestGroupBox, boxHeight, SWP_NOZORDER);
    hDeferedWindows = DeferWindowPos(hDeferedWindows, groupStats[2], HWND_NOTOPMOST, 0, boxHeight*2, widestGroupBox, boxHeight, SWP_NOZORDER);
    
    hDeferedWindows = DeferWindowPos(hDeferedWindows, scoreLabel.getHandle(), HWND_NOTOPMOST, CS.XGROUPBOX_MARGIN, CS.YFIRST_GROUPBOX_MARGIN, widestGroupBox - (CS.XGROUPBOX_MARGIN * 2), CD.YLABEL, SWP_NOZORDER);
    hDeferedWindows = DeferWindowPos(hDeferedWindows, pointsLabel.getHandle(), HWND_NOTOPMOST, CS.XGROUPBOX_MARGIN, CS.YFIRST_GROUPBOX_MARGIN + (boxHeight), widestGroupBox - (CS.XGROUPBOX_MARGIN * 2), CD.YLABEL, SWP_NOZORDER);
    hDeferedWindows = DeferWindowPos(hDeferedWindows, timeLabel.getHandle(), HWND_NOTOPMOST, CS.XGROUPBOX_MARGIN, CS.YFIRST_GROUPBOX_MARGIN + (boxHeight * 2), widestGroupBox - (CS.XGROUPBOX_MARGIN * 2), CD.YLABEL, SWP_NOZORDER);
    
    hDeferedWindows = DeferWindowPos(hDeferedWindows, buttonPause, HWND_NOTOPMOST, 0, boxHeight * 3, widestGroupBox, tallestPoint - (boxHeight * 3), SWP_NOZORDER);
    hDeferedWindows = DeferWindowPos(hDeferedWindows, gamePanel.getHandle(), HWND_NOTOPMOST, widestGroupBox, 0, (gamePanel.getTilesetHeight() * horzTiles), tallestPoint, SWP_NOZORDER);

    EndDeferWindowPos(hDeferedWindows);

    centerWindow();
    
}

//------------------------------------------------------------------------------
// onTileSelected - Processes the UWM_TILE_SELECTED message
//------------------------------------------------------------------------------

void MainWindowView::onTileSelected(const WPARAM& wParam, const LPARAM& lParam) {
    
    uint8_t xPos = static_cast<uint8_t>(wParam / gamePanel.getTilesetHeight());
    uint8_t yPos = static_cast<uint8_t>(lParam / gamePanel.getTilesetHeight());

    const int retVal = windowPresenter.tryFlipTileAtCoodinates(xPos, yPos);

    if(retVal > GameBoardFlipErrors::WasSuccessful) {
        
        InvalidateRect(gamePanel.getHandle(), NULL, TRUE);

        if(retVal == GameBoardFlipErrors::TilesNotMatched) {
            // Set/Reset timer
            SetTimer(hWnd, MainWindowViewConstants::FLIP_TIMER_ID, 1000, NULL);
            shouldUnflip = true;

        }
        else if(retVal == GameBoardFlipErrors::TilesMatched) {

            updateScore(scoreLabel.getHandle(), windowPresenter.getScore());
            updatePoints(pointsLabel.getHandle(), windowPresenter.getPoints());

        }

    }
}

//------------------------------------------------------------------------------
// onElapsedTimeTimer - Processes the WM_TIMER event with the ELAPSED_TIMER_ID
// ID.
//------------------------------------------------------------------------------

void MainWindowView::onElapsedTimeTimer() {
    
    // TODO: tidy this up
    DWORD elapsedTime = windowPresenter.getElapsedTime();

    DWORD seconds = elapsedTime / 1000;
    DWORD minutes = seconds / 60;
    seconds = seconds - (minutes * 60);

    wchar_t timeStr[32] = {0};
    wsprintf(timeStr, L"%02d:%02d", minutes, seconds);
    SetWindowText(timeLabel.getHandle(), timeStr);

    updatePoints(pointsLabel.getHandle(), windowPresenter.getPoints());

}

void MainWindowView::onChangeBoardSize(const int& menuID) {
    
    bool retVal = false;

    switch(menuID) {
        case MenuID::BOARD_3x3:
            retVal = windowPresenter.tryUpdateGameBoard(3, 3, WindowPresenterConstants::IGNORE_NUMTILES);
            break;
        case MenuID::BOARD_4x4:
            retVal = windowPresenter.tryUpdateGameBoard(4, 4, WindowPresenterConstants::IGNORE_NUMTILES);
            break;
        case MenuID::BOARD_5x5:
            retVal = windowPresenter.tryUpdateGameBoard(5, 5, WindowPresenterConstants::IGNORE_NUMTILES);
            break;
        case MenuID::BOARD_5x9:
            retVal = windowPresenter.tryUpdateGameBoard(5, 9, WindowPresenterConstants::IGNORE_NUMTILES);
            break;
        case MenuID::BOARD_10x10:
            retVal = windowPresenter.tryUpdateGameBoard(10, 10, WindowPresenterConstants::IGNORE_NUMTILES);
            break;
    }

    if(retVal) {
        for(int i = MenuID::BOARD_3x3; i <= MenuID::BOARD_10x10; ++i) {

            if(i != menuID) {
                CheckMenuItem(optionsMenu, i, MainWindowViewConstants::MENU_UNCHECKED_FLAGS);
            }
            else {
                CheckMenuItem(optionsMenu, i, MainWindowViewConstants::MENU_CHECKED_FLAGS);
            }

        }
    }

}

//------------------------------------------------------------------------------
// windowProc - Standard window procedure for a window
//------------------------------------------------------------------------------

LRESULT MainWindowView::windowProc(const UINT& msg, const WPARAM wParam, const LPARAM lParam) {

    switch(msg) {
        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
        case WM_TIMER:
            if(wParam == MainWindowViewConstants::ELAPSED_TIMER_ID) {
                onElapsedTimeTimer();
            }
            else if(wParam == MainWindowViewConstants::FLIP_TIMER_ID) {
                if(shouldUnflip) {
                    windowPresenter.unflipTiles();
                    shouldUnflip = false;
                    InvalidateRect(gamePanel.getHandle(), NULL, FALSE);
                }
            }
            break;

        case WM_CREATE:
            onCreate();
            break;

        case WM_COMMAND:

            switch (wParam) {

                case MenuID::NEW_GAME:
                    if(windowPresenter.tryNewGame()) {
                        onElapsedTimeTimer(); // TODO: Might be beneficial to inline this
                        updatePoints(pointsLabel.getHandle(), windowPresenter.getPoints());
                        updateScore(scoreLabel.getHandle(), windowPresenter.getScore());
                    }
                    break;

                case CtrlID::BUTTON_PAUSE: 
                case MenuID::PAUSE_GAME:
                    windowPresenter.tryTogglePause(); 
                    break;

                case MenuID::BOARD_3x3:
                case MenuID::BOARD_4x4:
                case MenuID::BOARD_5x5:
                case MenuID::BOARD_5x9:
                case MenuID::BOARD_10x10:
                    onChangeBoardSize(wParam);
                    break;

                case MenuID::ABOUT:
                    aboutWindow.createWindow(GetModuleHandle(NULL), hWnd);
                    break;
                
            }

            break;

        case UWM_TILE_SELECTED:
            onTileSelected(wParam, lParam);
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

//==============================================================================
// Interface Functions
//==============================================================================

//-----------------------------------------------------------------------------
// implAskYesNoQuestion - Ask the user for a yes, or a no
//-----------------------------------------------------------------------------

int MainWindowView::implAskYesNoQuestion(const wchar_t* message, const wchar_t* title) {
    return MessageBox(hWnd, message, title, MB_YESNOCANCEL | MB_ICONQUESTION);
}

//-----------------------------------------------------------------------------
// implGameStateChanged - Game state was changed
//-----------------------------------------------------------------------------

void MainWindowView::implGameStateChanged(const int& newState) {
    if(newState == GameState::STATE_PLAYING) {
        EnableWindow(buttonPause, TRUE);
        ModifyMenu(fileMenu, MenuID::PAUSE_GAME, MainWindowViewConstants::MENU_ENABLED_FLAGS, MenuID::PAUSE_GAME, L"&Pause");
        SetWindowTextW(buttonPause, GET_LANG_STR(LangID::PAUSE_BUTTON_CAPTION));
    }
    else if(newState == GameState::STATE_GAMEWON || newState == GameState::STATE_NOT_STARTED) {
        EnableWindow(buttonPause, FALSE);
        ModifyMenu(fileMenu, MenuID::PAUSE_GAME, MainWindowViewConstants::MENU_DISABLED_FLAGS, MenuID::PAUSE_GAME, L"&Pause");

        if(newState == GameState::STATE_NOT_STARTED) {
            moveControls();
        }

        InvalidateRect(gamePanel.getHandle(), NULL, FALSE);
    }
    else if(newState == GameState::STATE_PAUSED) {
        SetWindowTextW(buttonPause, GET_LANG_STR(LangID::UNPAUSE_BUTTON_CAPTION));
        ModifyMenu(fileMenu, MenuID::PAUSE_GAME, MainWindowViewConstants::MENU_ENABLED_FLAGS, MenuID::PAUSE_GAME, L"Un&pause");
    }

}