#include "game_panel.h"
#include "subclassed_controls.h"
#include "shared_functions.h"

bool GamePanel::isRegistered = false;

//-----------------------------------------------------------------------------
// createWindow - Creates the panel that will contain the game board
//-----------------------------------------------------------------------------

// TODO: X,Y,W,H, Style Flags

bool GamePanel::createWindow(const HINSTANCE& hInst, const HWND& parent, const HMENU& ID) {

    if(hWnd) {
        return true; // Already created.
    }

    hWnd = CreateWindowEx(0, L"GamePanel", L"",
        WS_CHILD | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 192, 320,
        parent, ID, hInst, this);

    if(hWnd == NULL) {
        MessageBox(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return false;
    }

    // TODO: This should be a function
    tilesetBMP = (HBITMAP)LoadImage(NULL, L"tileset.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE); 
    BITMAP bmInfo = {0};
    GetObject(tilesetBMP, sizeof(BITMAP), &bmInfo);
    bmpWidth    = bmInfo.bmWidth;
    bmpHeight   = bmInfo.bmHeight;

    // TODO: MainWindowPresenter stuff?

    ShowWindow(hWnd, SW_NORMAL);
    UpdateWindow(hWnd);

    return true;
}

//-----------------------------------------------------------------------------
// registerSelf - Registers the Window class.
//-----------------------------------------------------------------------------

bool GamePanel::registerSelf(HINSTANCE hInst) {
    
    if(isRegistered) {
        MessageBox(NULL, L"Control 'GamePanel' is already registered!", L"Already Registered", MB_OK);
        return false;
    }

    WNDCLASSEX wc;

    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = GamePanel::WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInst;
    wc.hIcon         = NULL;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(RGB(0,0,0));
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = L"GamePanel";
    wc.hIconSm       = NULL;

    if(!RegisterClassEx(&wc)) {
        MessageBox(NULL, L"Error registering window class.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    isRegistered = true;

    return true;
}



//-----------------------------------------------------------------------------
// windowProc - Standard window procedure for a window
//-----------------------------------------------------------------------------

LRESULT GamePanel::windowProc(const UINT& msg, const WPARAM wParam, const LPARAM lParam) {

    switch(msg) {
        
        default: return DefWindowProc(hWnd, msg, wParam, lParam);
        case WM_LBUTTONDOWN: SendMessage(GetParent(hWnd), UWM_TILE_SELECTED, LOWORD(lParam), HIWORD(lParam)); break;
        case WM_PAINT:      onPaint(); break;
        case WM_ERASEBKGND: return -1; break; // We will redraw everything in WM_PAINT
        case WM_CLOSE:      DestroyWindow(hWnd); break;
    }
    return 0;
}

//-----------------------------------------------------------------------------
// onPaint - Processing of the WM_PAINT message
//-----------------------------------------------------------------------------

#define DX2_TO_DX1(i, k, width)    ( (k * width) + i)

void GamePanel::onPaint() {

    RECT rc;
    GetClientRect(hWnd, &rc);

    PAINTSTRUCT ps;
    if(!BeginPaint(hWnd, &ps)) {
        // TODO: Begin Paint failed.
        return;
    }

    HDC         backBuffer;
    HBITMAP     backBitmap, prevBitmap;
    HDC         tilesetHDC;

    // Create a back buffer
    // TODO: Only need to recreate after WM_DISPLAYCHANGE, and maybe WM_FONTCHANGE 
    // Also we only need to draw to the screen, that which has been invalidated, and we don't need
    // to erase the background every time
    createBackBuffer(ps.hdc, backBuffer, backBitmap, prevBitmap, rc);
    
    FillRect(backBuffer, &rc, (HBRUSH)GetClassLongPtr(hWnd, GCLP_HBRBACKGROUND));
    
    tilesetHDC = CreateCompatibleDC(ps.hdc);
    HBITMAP tempBMP = (HBITMAP)SelectObject(tilesetHDC, tilesetBMP);

    // TODO: This is obviously not complete

    const GameBoard& gameBoard = windowPresenter->getGameBoard();

    const uint8_t& boardWidth = gameBoard.getWidth();
    const uint8_t& boardHeight = gameBoard.getHeight();


    const std::vector<TileData>& tiles = gameBoard.getTiles();

    for(uint8_t k = 0; k < boardHeight; ++k) {
        for(uint8_t i = 0; i < boardWidth; ++i) {
            if(!tiles[DX2_TO_DX1(i, k, boardWidth)].flags) {
                BitBlt(backBuffer, i * bmpHeight, k * bmpHeight, bmpHeight, bmpHeight, tilesetHDC, 0, 0, SRCCOPY);
            }
            else {
                BitBlt(backBuffer, i * bmpHeight, k * bmpHeight, bmpHeight, bmpHeight, tilesetHDC, bmpHeight * tiles[DX2_TO_DX1(i, k, boardWidth)].ID, 0, SRCCOPY);
            }
        }
    }

    SelectObject(tilesetHDC, tempBMP);
    DeleteDC(tilesetHDC);

    // Copy back buffer to DC
    BitBlt(ps.hdc, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, backBuffer, 0, 0, SRCCOPY);

    // Destroy BackBuffer
    destroyBackBuffer(backBuffer, backBitmap, prevBitmap);

    EndPaint(hWnd, &ps);
}

#undef DX2_TO_DX1