#include "gameboard_panel.h"

//=============================================================================
// Accessors
//=============================================================================

//-----------------------------------------------------------------------------
// getWndClassName - Get the class name associated with this window
//-----------------------------------------------------------------------------

wchar_t* GameBoardPanel::getWndClassName() {
    return L"GameBoardClass";
}

//=============================================================================
// Mutators
//=============================================================================

//-----------------------------------------------------------------------------
// setTileset - Change the handle of the tileset image
//-----------------------------------------------------------------------------

void GameBoardPanel::setTileset(const HBITMAP newTileset) {
    tileset = newTileset;
}

//=============================================================================
// Public Functions
//=============================================================================

//-----------------------------------------------------------------------------
// registerSelf - Register the Gameboard's window class
//-----------------------------------------------------------------------------

bool GameBoardPanel::registerSelf(HINSTANCE hInstance) {

    WNDCLASSEX boardClass = {0};

    boardClass.cbSize        = sizeof(WNDCLASSEX);
    boardClass.lpfnWndProc   = GameBoardPanel::WndProc;
    boardClass.hInstance     = hInstance;
    boardClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    boardClass.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
    boardClass.lpszClassName = GameBoardPanel::getWndClassName();

    if(!RegisterClassEx(&boardClass)) {
        MessageBox(NULL, L"Error registering Game Board class.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// windowProc - Window procedure for the window.
//-----------------------------------------------------------------------------

LRESULT GameBoardPanel::windowProc(const UINT& msg, const WPARAM wParam, const LPARAM lParam) {

    switch(msg) {
        case WM_LBUTTONDOWN:
            PostMessage(parent, BP_CLICKED, LOWORD(lParam), HIWORD(lParam));
            return 0;
        case WM_CREATE: return onCreate(reinterpret_cast<CREATESTRUCT*>(lParam));
        case WM_PAINT:  return onPaint();
    }

    return DefWindowProc(window, msg, wParam, lParam);
}

//=============================================================================
// Private Functions
//=============================================================================

//-----------------------------------------------------------------------------
// onCreate - Processes the WM_CREATE event. We create the sources we need here
//-----------------------------------------------------------------------------

LRESULT GameBoardPanel::onCreate(const CREATESTRUCT* const cs) {
    gameBG = CreateSolidBrush(RGB(0, 0, 0));
    parent = cs->hwndParent;
    return 0;
}

//-----------------------------------------------------------------------------
// onPaint - Processes the WM_PAINT window message by painting the game board
//-----------------------------------------------------------------------------

LRESULT GameBoardPanel::onPaint() {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(window, &ps);
    RECT rc; 

    GetClientRect(window, &rc);
    rc.left = 0;

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
                    BitBlt(hdc, i * 32, k * 32, 32, 32, hdcSrc, 512, 0, SRCCOPY);
                }
                else {
                    BitBlt(hdc, i * 32, k * 32, 32, 32, hdcSrc, 32 * (gameTiles[(k * width) + i].tileType - 1), 0, SRCCOPY);
                }
            }
        }

        SelectObject(hdcSrc, oldBMP);
    }

    EndPaint(window, &ps);
    return 0;
}
