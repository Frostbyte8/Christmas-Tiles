#include "main_window.h"
#include "subclassed_controls.h"
#include "../../language_table.h"
#include "../../resources/resource.h"
#include "shared_functions.h"
#include "../../constants.h"
#include "../../frost_util.h"

//==============================================================================
// Namespaces / Enums / Constants
//==============================================================================

static const DWORD WINDOW_STYLE = (WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_THICKFRAME));
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

__forceinline BOOL DoesFileExist(const wchar_t* path) {
    const DWORD fileAttrib = GetFileAttributes(path);
    return (fileAttrib != INVALID_FILE_ATTRIBUTES && !(fileAttrib & FILE_ATTRIBUTE_DIRECTORY));
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

    hWnd = CreateWindowEx(WINDOW_STYLE_EX, L"XmasTilesMainWindow", GET_LANG_STR(LangID::APP_TITLE),
        WINDOW_STYLE, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0,
        NULL, NULL, hInstance, this);

    if(hWnd == NULL) {
        MessageBox(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return false;
    }

    prevMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);

    // TODO: This should be set for the first time when the game starts?
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
        else if(enterScoreWindow.getHandle() && IsDialogMessage(enterScoreWindow.getHandle(), &msg)) {
        }
        else if(highscoresWindow.getHandle() && IsDialogMessage(highscoresWindow.getHandle(), &msg)) {
        }
        else if(customSizeWindow.getHandle() && IsDialogMessage(customSizeWindow.getHandle(), &msg)) {
        }
        else if (!IsDialogMessage(hWnd, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
 
    }

    windowPresenter.~MainWindowPresenter();

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

    AppendMenu(fileMenu, MF_STRING, MenuID::NEW_GAME, GET_LANG_STR(LangID::MENU_NEW_GAME));
    AppendMenu(fileMenu, MF_STRING | MF_DISABLED | MF_GRAYED, MenuID::PAUSE_GAME, GET_LANG_STR(LangID::MENU_PAUSE));
    AppendMenu(fileMenu, MF_SEPARATOR, 0, 0);
    AppendMenu(fileMenu, MF_STRING, MenuID::HIGHSCORES, GET_LANG_STR(LangID::MENU_HIGHSCORES));
    AppendMenu(fileMenu, MF_SEPARATOR, 0, 0);
    AppendMenu(fileMenu, MF_STRING, MenuID::EXIT, GET_LANG_STR(LangID::MENU_EXIT));

    // Options Menu
    AppendMenu(optionsMenu, MF_STRING, MenuID::TILESET, GET_LANG_STR(LangID::MENU_CHANGE_TILESET));
    AppendMenu(optionsMenu, MF_STRING, MenuID::CHANGE_LANGUAGE, GET_LANG_STR(LangID::MENU_CHANGE_LANGUAGE));
    AppendMenu(optionsMenu, MF_SEPARATOR, 0, 0);
    AppendMenu(optionsMenu, MF_STRING, MenuID::BOARD_3x3, L"3x3");
    AppendMenu(optionsMenu, MF_STRING, MenuID::BOARD_4x4, L"4x4");
    AppendMenu(optionsMenu, MF_STRING, MenuID::BOARD_5x5, L"5x5");
    AppendMenu(optionsMenu, MF_STRING, MenuID::BOARD_5x9, L"5x9");
    AppendMenu(optionsMenu, MF_STRING, MenuID::BOARD_10x10, L"10x10");
    AppendMenu(optionsMenu, MF_STRING, MenuID::BOARD_CUSTOM, GET_LANG_STR(LangID::MENU_CUSTOM_SIZE));

    // Help Meun
    AppendMenu(helpMenu, MF_STRING, MenuID::HELP_FILE, GET_LANG_STR(LangID::MENU_HELP_ITEM));
    AppendMenu(helpMenu, MF_SEPARATOR, 0, 0);
    AppendMenu(helpMenu, MF_STRING, MenuID::ABOUT, GET_LANG_STR(LangID::MENU_ABOUT));

    // Now attach each menu to the menu bar
    AppendMenu(menuBar, MF_STRING | MF_POPUP, reinterpret_cast<UINT_PTR>(fileMenu), GET_LANG_STR(LangID::MENU_FILE));
    AppendMenu(menuBar, MF_STRING | MF_POPUP, reinterpret_cast<UINT_PTR>(optionsMenu), GET_LANG_STR(LangID::MENU_OPTIONS));
    AppendMenu(menuBar, MF_STRING | MF_POPUP, reinterpret_cast<UINT_PTR>(helpMenu), GET_LANG_STR(LangID::MENU_HELP));

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

    // TODO: Retriving these settings should be in the presenter, not the view.

    uint8_t boardWidth = ChristmasTilesConstants::DEFAULT_BOARD_WIDTH;
    uint8_t boardHeight = ChristmasTilesConstants::DEFAULT_BOARD_HEIGHT;
    wchar_t filePath[MAX_PATH] = {0};

    if(!DoesFileExist(L".\\ChristmasTiles.ini")) {

        // Board Dimensions
        wchar_t buffer[4] = {0};
        wsprintf(buffer, L"%d", boardWidth);
        WritePrivateProfileStringW(L"settings", L"width", buffer, L".\\ChristmasTiles.ini");
        wsprintf(buffer, L"%d", boardHeight);
        WritePrivateProfileStringW(L"settings", L"height", buffer, L".\\ChristmasTiles.ini");
        
        // Tileset Options
        WritePrivateProfileStringW(L"settings", L"tileset", L"", L".\\ChristmasTiles.ini");

    }
    else {
    
        boardWidth = static_cast<uint8_t>(GetPrivateProfileInt(L"settings", L"width", ChristmasTilesConstants::DEFAULT_BOARD_WIDTH, L".\\ChristmasTiles.ini"));
        boardHeight = static_cast<uint8_t>(GetPrivateProfileInt(L"settings", L"height", ChristmasTilesConstants::DEFAULT_BOARD_HEIGHT, L".\\ChristmasTiles.ini"));
        GetPrivateProfileString(L"settings", L"tileset", NULL, filePath, MAX_PATH-1, L".\\ChristmasTiles.ini");

        boardWidth = FrostUtil::ClampInts(boardWidth, GameBoardConstants::MIN_WIDTH, GameBoardConstants::MAX_WIDTH);
        boardHeight = FrostUtil::ClampInts(boardHeight, GameBoardConstants::MIN_HEIGHT, GameBoardConstants::MAX_HEIGHT);

    }

    // TODO: gamePanel should get the presenter via it's constructor?
    gamePanel.createWindow(hInstance, hWnd, reinterpret_cast<HMENU>(CtrlID::PANEL_GAMEBOARD), filePath);
    gamePanel.setWindowPresenter(&windowPresenter);
    // TODO: (gamePanel.getTilesetWidth() / gamePanel.getTilesetHeight()) - 2)
    windowPresenter.tryUpdateGameBoard(boardWidth, boardHeight, static_cast<uint8_t>( (gamePanel.getTilesetWidth() / gamePanel.getTilesetHeight()) - 2));
    windowPresenter.readScores(); // TODO: This should be somewhere else.

    updateBoardSizeMenu(boardWidth, boardHeight, false);

    metrics.initWindowMetrics();

    HFONT dialogFont = metrics.GetCurrentFont();
    EnumChildWindows(hWnd, reinterpret_cast<WNDENUMPROC>(ChangeControlsFont), (LPARAM)dialogFont);
    prevMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);

    moveControls();

    // Now that the window is done being created, move and show it.
    // This is to reduce flicker.
    ShowWindow(hWnd, SW_NORMAL);
    UpdateWindow(hWnd);

    return true;

}

//==============================================================================
// Private Functions
//==============================================================================

//------------------------------------------------------------------------------
// onWindowMoved - Track which monitor the window is on after it has been moved,
// and if it has changed which monitor it is on center it.
//------------------------------------------------------------------------------

void MainWindowView::onWindowMoved() {
    HMONITOR currentMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);

    if(currentMonitor != prevMonitor) {
        prevMonitor = currentMonitor;

        // TODO: Reinit Window Metrics, and reset fonts, if DPI has actually changed.

        moveControls(); // This will also center it

    }
    
}

//------------------------------------------------------------------------------
// getTallestPoint - Obtain the tallest point of the window.
//------------------------------------------------------------------------------

// TODO: Tallest and Widest could be merged into one function.

LONG MainWindowView::getTallestPoint() const {
    const ControlDimensions& CD = metrics.getControlDimensions();
    const ControlSpacing& CS = metrics.getControlSpacing();

    const long minHeight = gamePanel.getTilesetHeight() * 9;
    const long boardheight = gamePanel.getTilesetHeight() * windowPresenter.getGameBoard().getBoardDimensions().height;


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

    // TODO: This for some reason is called twice, figure out why.

    const ControlDimensions& CD = metrics.getControlDimensions();
    const ControlSpacing& CS = metrics.getControlSpacing();

    // TODO: This entire code needs to be rewritten.
    // Client Area Width is Widest GroupBox + GameBoard Width
    // Client Area Height is whatever is taller: 3 Groupboxes + Button, or Gameboard
    // Min Width: GroupBox + at least 5 tiles
    // Min Height: No less than 9 tiles tall.

    // TODO: Scroll Bars when the width/height gets too wide.
    const LONG tallestPoint = getTallestPoint();
    const LONG widestGroupBox = getWidestGroupBox();

    // TODO: This may not need to actually be here

    // Resize Window
    // TODO: Get Monitor Window is on?

    int horzTiles = windowPresenter.getGameBoard().getBoardDimensions().width;
    if(horzTiles < 5) {
        horzTiles = 5;
    }

    RECT rc = {0, 0, (gamePanel.getTilesetHeight() * horzTiles) + widestGroupBox, tallestPoint};

    // TODO: This is just for testing scroll bars. set it to a tiny size to try it out

    MONITORINFO mi = {0};
    mi.cbSize = sizeof(MONITORINFO);
    GetMonitorInfo(prevMonitor, &mi);
    
    if(rc.right > abs(mi.rcMonitor.right - mi.rcMonitor.left) * 0.80) {
        rc.right = static_cast<LONG>(abs(mi.rcMonitor.right - mi.rcMonitor.right) * 0.80);
    }

    if(rc.bottom > abs(mi.rcMonitor.bottom - mi.rcMonitor.top) * 0.80) {
        rc.bottom = static_cast<LONG>(abs(mi.rcMonitor.bottom - mi.rcMonitor.top) * 0.80);
    }

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

    // 

    hDeferedWindows = DeferWindowPos(hDeferedWindows, gamePanel.getHandle(), HWND_NOTOPMOST, widestGroupBox, 0, rc.right - widestGroupBox, tallestPoint, SWP_NOZORDER);

    EndDeferWindowPos(hDeferedWindows);
    AdjustWindowRectEx(&rc, WINDOW_STYLE, TRUE, WINDOW_STYLE_EX);
    // TODO: Prev monitor tracking
    CenterWindow(hWnd, rc, prevMonitor);

    // Revalidate Game Panel, but then invalidate it only to repaint, not erase it.
    GetClientRect(hWnd, &rc);
    rc.left = widestGroupBox;
    ValidateRect(hWnd, &rc);
    InvalidateRect(hWnd, &rc, FALSE);
}

//------------------------------------------------------------------------------
// onTileSelected - Processes the UWM_TILE_SELECTED message
//------------------------------------------------------------------------------

void MainWindowView::onTileSelected(const WPARAM& wParam, const LPARAM& lParam) {
    
    unsigned int xPos = wParam / gamePanel.getTilesetHeight();
    unsigned int yPos = lParam / gamePanel.getTilesetHeight();

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
// onClose - User wants to quit the program
//------------------------------------------------------------------------------

// The while loops are fine, so we can disable this
#pragma warning (push) 
#pragma warning (disable: 4127)

void MainWindowView::onClose() {

    if(windowPresenter.getGameState() != GameState::STATE_GAMEWON) {
        if(implAskYesNoQuestion(GET_LANG_STR(LangID::GAME_IN_PROGRESS_QUIT_TEXT), GET_LANG_STR(LangID::GAME_IN_PROGRESS_QUIT_TITLE)) != MainWindowInterfaceResponses::YES) {
            return;
        }
    }

    while (true) {
        
        if(windowPresenter.writeSettings()) {
            break;
        }

        // TODO: Language String
        if(MessageBox(hWnd, L"Error writing settings file.", L"Error", MB_ICONERROR | MB_RETRYCANCEL) == IDCANCEL) {
            break;
        }

    }

    while (true) {
        
        if(windowPresenter.writeScores()) {
            break;
        }

        // TODO: Language String
        if(MessageBox(hWnd, L"Error writing scores file.", L"Error", MB_ICONERROR | MB_RETRYCANCEL) == IDCANCEL) {
            break;
        }

    }


    KillTimer(hWnd, 1);
    DestroyWindow(hWnd);
}

#pragma warning (pop) 

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

//------------------------------------------------------------------------------
// onChangeBoardSize - User wants to change the size of the game board
//------------------------------------------------------------------------------

void MainWindowView::onChangeBoardSize(const int& menuID) {

    bool retVal = false;

    switch(menuID) {
        case MenuID::BOARD_3x3:
            retVal = windowPresenter.tryUpdateGameBoard(3, 3, WindowPresenterConstants::IGNORE_NUMTILES);
            updateBoardSizeMenu(3,3,true);
            break;
        case MenuID::BOARD_4x4:
            retVal = windowPresenter.tryUpdateGameBoard(4, 4, WindowPresenterConstants::IGNORE_NUMTILES);
            updateBoardSizeMenu(4,4,true);
            break;
        case MenuID::BOARD_5x5:
            retVal = windowPresenter.tryUpdateGameBoard(5, 5, WindowPresenterConstants::IGNORE_NUMTILES);
            updateBoardSizeMenu(5,5,true);
            break;
        case MenuID::BOARD_5x9:
            retVal = windowPresenter.tryUpdateGameBoard(5, 9, WindowPresenterConstants::IGNORE_NUMTILES);
            updateBoardSizeMenu(5,9,true);
            break;
        case MenuID::BOARD_10x10:
            retVal = windowPresenter.tryUpdateGameBoard(10, 10, WindowPresenterConstants::IGNORE_NUMTILES);
            updateBoardSizeMenu(10,10,true);
            break;
    }    

}

void MainWindowView::updateBoardSizeMenu(const uint8_t& width, const uint8_t& height, bool clearChecks) {

    if(clearChecks) {
        for(int i = MenuID::BOARD_3x3; i <= MenuID::BOARD_CUSTOM; ++i) {
            CheckMenuItem(optionsMenu, i, MainWindowViewConstants::MENU_UNCHECKED_FLAGS);
        }
    }

    if(width == 3 && height == 3) {
        CheckMenuItem(optionsMenu, MenuID::BOARD_3x3, MainWindowViewConstants::MENU_CHECKED_FLAGS);
    }
    else if(width == 4 && height == 4) {
        CheckMenuItem(optionsMenu, MenuID::BOARD_4x4, MainWindowViewConstants::MENU_CHECKED_FLAGS);
    }
    else if(width == 5 && height == 5) {
        CheckMenuItem(optionsMenu, MenuID::BOARD_5x5, MainWindowViewConstants::MENU_CHECKED_FLAGS);
    }
    else if(width == 5 && height == 9) {
        CheckMenuItem(optionsMenu, MenuID::BOARD_5x9, MainWindowViewConstants::MENU_CHECKED_FLAGS);
    }
    else if(width == 10 && height == 10) {
        CheckMenuItem(optionsMenu, MenuID::BOARD_10x10, MainWindowViewConstants::MENU_CHECKED_FLAGS);
    }
    else {
        CheckMenuItem(optionsMenu, MenuID::BOARD_CUSTOM, MainWindowViewConstants::MENU_CHECKED_FLAGS);
    }

}

void MainWindowView::onChangeTileset() {
    
    // TODO: Warn user about this action starting a new game
    
    OPENFILENAME ofnTileset = {0};
    wchar_t filePath[MAX_PATH] = L"";

    ofnTileset.lStructSize = sizeof(ofnTileset);
    ofnTileset.hwndOwner = hWnd;

    // TODO: LANG ID for this
    ofnTileset.lpstrFilter = L"Bitmap Files (*.bmp)\0*.bmp\0";
    ofnTileset.nMaxFile = MAX_PATH;
    ofnTileset.lpstrFile = filePath;
    ofnTileset.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofnTileset.lpstrDefExt = L"bmp";

    if(GetOpenFileName(&ofnTileset)) {

        if(gamePanel.changeTileset(ofnTileset.lpstrFile)) {
            // TODO: (gamePanel.getTilesetWidth() / gamePanel.getTilesetHeight()) - 2) is a bad way to do this.
            // It could also pose a problem and needs to have proper error checking.
            windowPresenter.tryUpdateGameBoard(WindowPresenterConstants::IGNORE_WIDTH, WindowPresenterConstants::IGNORE_HEIGHT, static_cast<uint8_t>((gamePanel.getTilesetWidth() / gamePanel.getTilesetHeight()) - 2));
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
        case WM_EXITSIZEMOVE:
            onWindowMoved(); 
            break;
        case WM_CREATE:
            onCreate();
            break;

        case WM_COMMAND:

            switch (wParam) {

                case MenuID::NEW_GAME:
                    if(windowPresenter.requestNewGame()) {
                        onElapsedTimeTimer();
                        updatePoints(pointsLabel.getHandle(), windowPresenter.getPoints());
                        updateScore(scoreLabel.getHandle(), windowPresenter.getScore());
                    }
                    break;

                case CtrlID::BUTTON_PAUSE: 
                case MenuID::PAUSE_GAME:
                    windowPresenter.tryTogglePause(); 
                    break;

                case MenuID::TILESET:
                    onChangeTileset();
                    break;

                case MenuID::BOARD_3x3:
                case MenuID::BOARD_4x4:
                case MenuID::BOARD_5x5:
                case MenuID::BOARD_5x9:
                case MenuID::BOARD_10x10:
                    onChangeBoardSize(wParam);
                    break;

                case MenuID::BOARD_CUSTOM:
                    customSizeWindow.createWindow(GetModuleHandle(NULL), hWnd);
                    break;
                
                case MenuID::HIGHSCORES:
                    highscoresWindow.createWindow(GetModuleHandle(NULL), hWnd, windowPresenter.getScoreTable());
                    break;

                case MenuID::ABOUT:
                    aboutWindow.createWindow(GetModuleHandle(NULL), hWnd);
                    break;

                case MenuID::EXIT:
                    onClose();
                    break;
                
            }

            break;

        case UWM_TILE_SELECTED:
            onTileSelected(wParam, lParam);
            break;

        case UWM_SCORE_ENTERED:
            windowPresenter.tryAddScore(enterScoreWindow.getName(), 9001); // TODO: Index //, windowPresenter.getScore(), 2025, 11, 01, 9001); // TODO: Index
            highscoresWindow.createWindow(GetModuleHandle(NULL), hWnd, windowPresenter.getScoreTable());
            break;

        case UWM_CUSTOM_SIZE_ENETERD:
            windowPresenter.tryUpdateGameBoard(customSizeWindow.getNewWidth() , customSizeWindow.getNewHeight(), WindowPresenterConstants::IGNORE_NUMTILES);
            updateBoardSizeMenu(customSizeWindow.getNewWidth(), customSizeWindow.getNewHeight(), true);
            break;

        case WM_CLOSE:
            onClose();
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
        ModifyMenu(fileMenu, MenuID::PAUSE_GAME, MainWindowViewConstants::MENU_ENABLED_FLAGS, MenuID::PAUSE_GAME, GET_LANG_STR(LangID::MENU_PAUSE));
        SetWindowTextW(buttonPause, GET_LANG_STR(LangID::PAUSE_BUTTON_CAPTION));
    }
    else if(newState == GameState::STATE_GAMEWON || newState == GameState::STATE_NOT_STARTED) {
        EnableWindow(buttonPause, FALSE);
        ModifyMenu(fileMenu, MenuID::PAUSE_GAME, MainWindowViewConstants::MENU_DISABLED_FLAGS, MenuID::PAUSE_GAME, GET_LANG_STR(LangID::MENU_PAUSE));
        InvalidateRect(gamePanel.getHandle(), NULL, FALSE);

        if(newState == GameState::STATE_GAMEWON) {

            size_t isNewScore = windowPresenter.getScoreTable().isNewHighscore(windowPresenter.getScore());

            if(isNewScore < 10) {
                enterScoreWindow.createWindow(GetModuleHandle(NULL), hWnd, isNewScore);
            }

        }
        else {
            const GameBoard& gameBoard = windowPresenter.getGameBoard();
            // TODO: Stop this from running the first time, the window is already moved.
            moveControls();
            const GameBoardDimensions& boardDimensions = gameBoard.getBoardDimensions();
            gamePanel.updateVirtualSize(boardDimensions.width, boardDimensions.height);
        }

    }
    else if(newState == GameState::STATE_PAUSED) {
        SetWindowTextW(buttonPause, GET_LANG_STR(LangID::UNPAUSE_BUTTON_CAPTION));
        ModifyMenu(fileMenu, MenuID::PAUSE_GAME, MainWindowViewConstants::MENU_ENABLED_FLAGS, MenuID::PAUSE_GAME, GET_LANG_STR(LangID::MENU_UNPAUSE));
    }

}