#include "game_panel.h"
#include "subclassed_controls.h"
#include "shared_functions.h"
#include <math.h>

bool GamePanel::isRegistered = false;

namespace ScrollType {
    static const int NONE = 0;
    static const int HORZ = 1;
    static const int VERT = 2;
    static const int BOTH = HORZ | VERT;
}

//==============================================================================
// Constructors
//==============================================================================

GamePanel::GamePanel() : hWnd(NULL), tilesetBMP(0), windowPresenter(NULL), xOffset(0), yOffset(0),
virtualWidth(0), virtualHeight(0) {
}

//==============================================================================
// Functions
//==============================================================================

//------------------------------------------------------------------------------
// createWindow - Creates the panel that will contain the game board
//------------------------------------------------------------------------------

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

    ShowWindow(hWnd, SW_NORMAL);
    UpdateWindow(hWnd);

    return true;
}

//------------------------------------------------------------------------------
// registerSelf - Registers the Window class.
//------------------------------------------------------------------------------

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

void GamePanel::onSize(const WORD& newWidth, const WORD& newHeight) {

    // First, we need to figure out which scroll bars we need

    const int ScrollSpanX = GetSystemMetrics(SM_CXHSCROLL);
    const int ScrollSpanY = GetSystemMetrics(SM_CXVSCROLL);

    int finalWidth = newWidth;
    int finalHeight = newHeight;

    int whichBars = 0;

    if(newWidth < virtualWidth) {

        // We might also need the other scrollbar if the span of the scrollbar
        // makes the space small enough

        if(newHeight - ScrollSpanX < virtualHeight) {
            ShowScrollBar(hWnd, SB_BOTH, TRUE);
            whichBars = ScrollType::BOTH;
            finalHeight -= ScrollSpanX;
            finalWidth -= ScrollSpanY;
        }
        else {
            ShowScrollBar(hWnd, SB_HORZ, TRUE);
            ShowScrollBar(hWnd, SB_VERT, FALSE);
            whichBars = ScrollType::HORZ;
            finalHeight -= ScrollSpanX;
        }

    }
    else if(newHeight < virtualHeight) {
        
        if(newWidth - ScrollSpanY < virtualWidth) {
            ShowScrollBar(hWnd, SB_BOTH, TRUE);
            whichBars = ScrollType::BOTH;
            finalHeight -= ScrollSpanX;
            finalWidth -= ScrollSpanY;
        }
        else {
            ShowScrollBar(hWnd, SB_VERT, TRUE);
            ShowScrollBar(hWnd, SB_HORZ, FALSE);
            whichBars = ScrollType::VERT;
            finalWidth -= ScrollSpanY;
        }

    }
    else {
        ShowScrollBar(hWnd, SB_BOTH, FALSE);
        return;
    }

    if(whichBars & ScrollType::HORZ) {
        SetScrollRange(hWnd, SB_HORZ, 0, virtualWidth - finalWidth, TRUE);
    }

    if(whichBars & ScrollType::VERT) {
        SetScrollRange(hWnd, SB_VERT, 0, virtualHeight - finalHeight, TRUE);
    }

}

void GamePanel::updateVirtualSize(const WORD& newXTiles, const WORD& newYTiles) {
    virtualWidth = static_cast<WORD>(newXTiles * bmpHeight);
    virtualHeight = static_cast<WORD>(newYTiles * bmpHeight);
    xOffset = 0;
    yOffset = 0;
    SetScrollPos(hWnd, SB_HORZ, 0, TRUE);
    SetScrollPos(hWnd, SB_VERT, 0, TRUE);
}

void GamePanel::onHScroll(const WORD& dir) {

    int minRange = 0;
    int maxRange = 0;

    GetScrollRange(hWnd, SB_HORZ, &minRange, &maxRange);

    if(dir == SB_LINERIGHT) {
        if(xOffset < maxRange) {
            xOffset++;
            SetScrollPos(hWnd, SB_HORZ, xOffset, TRUE);
        }
    }
    else if(dir == SB_LINELEFT) {

        if(xOffset > 0) {
            xOffset--;
            SetScrollPos(hWnd, SB_HORZ, xOffset, TRUE);
        }
    }
    else {
        return;
    }
    InvalidateRect(hWnd, NULL, TRUE);

}

void GamePanel::onVScroll(const WORD& dir) {

    int minRange = 0;
    int maxRange = 0;

    GetScrollRange(hWnd, SB_VERT, &minRange, &maxRange);

    if(dir == SB_LINEDOWN) {
        if(yOffset < maxRange) {
            yOffset++;
            SetScrollPos(hWnd, SB_VERT, yOffset, TRUE);
        }
    }
    else if(dir == SB_LINEUP) {

        if(yOffset > 0) {
            yOffset--;
            SetScrollPos(hWnd, SB_VERT, yOffset, TRUE);
        }
    }
    else {
        return;
    }
    InvalidateRect(hWnd, NULL, TRUE);

}


//------------------------------------------------------------------------------
// windowProc - Standard window procedure for a window
//------------------------------------------------------------------------------

LRESULT GamePanel::windowProc(const UINT& msg, const WPARAM wParam, const LPARAM lParam) {

    switch(msg) {
        
        default: return DefWindowProc(hWnd, msg, wParam, lParam);
        case WM_HSCROLL: onHScroll(LOWORD(wParam)); break;
        case WM_VSCROLL: onVScroll(LOWORD(wParam)); break;
        case WM_SIZE: onSize(LOWORD(lParam), HIWORD(lParam)); break;
        case WM_LBUTTONDOWN: SendMessage(GetParent(hWnd), UWM_TILE_SELECTED, LOWORD(lParam) + xOffset, HIWORD(lParam) + yOffset); break;
        case WM_PAINT:      onPaint(); break;
        case WM_ERASEBKGND: return -1; break; // We will redraw everything in WM_PAINT
        case WM_CLOSE:      DestroyWindow(hWnd); break;
    }
    return 0;
}

//------------------------------------------------------------------------------
// onPaint - Processing of the WM_PAINT message
//------------------------------------------------------------------------------

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

    if(windowPresenter) {

        const GameBoard& gameBoard = windowPresenter->getGameBoard();
        const uint8_t& boardWidth = gameBoard.getWidth();
        const std::vector<TileData>& tiles = gameBoard.getTiles();

        const int minXTile = static_cast<int>(floor(static_cast<float>(xOffset) / bmpHeight));
        const int minYTile = static_cast<int>(floor(static_cast<float>(yOffset) / bmpHeight));

        int maxXTile = static_cast<int>(ceil(static_cast<float>(rc.right) / bmpHeight));
        int maxYTile = static_cast<int>(ceil(static_cast<float>(rc.bottom) / bmpHeight));

        if(maxXTile > gameBoard.getWidth()) {
            maxXTile = gameBoard.getWidth();
        }

        if(maxYTile >  gameBoard.getHeight()) {
            maxYTile = gameBoard.getHeight();
        }
        
        int xDest = 0;
        int yDest = 0;
        int wDest = bmpHeight;
        int hDest = bmpHeight;
        int srcY = (yOffset % bmpHeight);

        // Check and see if the last tile will go over the edge of the back buffer
        const bool xOverDraw = (bmpHeight - (xOffset % bmpHeight)) + ((maxXTile - (minXTile + 1)) * bmpHeight) > rc.right ? true : false;
        const bool yOverDraw = (bmpHeight - (yOffset % bmpHeight)) + ((maxYTile - (minYTile + 1)) * bmpHeight) > rc.bottom ? true : false;

        for(int k = minYTile; k < maxYTile; ++k) {

            int srcX = (xOffset % bmpHeight);
            wDest = bmpHeight;

            for(int i = minXTile; i < maxXTile; ++i) {

                // Note to self: MSVC does in fact optimize this!
                const int realIndex = DX2_TO_DX1(i, k, boardWidth);
                
                if(!tiles[realIndex].flags) {

                    BitBlt(backBuffer, xDest, yDest, wDest - srcX, hDest - srcY, tilesetHDC, srcX, srcY, SRCCOPY);

                }
                else {
                    BitBlt(backBuffer, xDest, yDest, wDest - srcX, hDest - srcY, tilesetHDC, (tiles[realIndex].ID * bmpHeight) + srcX, srcY, SRCCOPY);
                }

                xDest += bmpHeight - srcX;
                srcX = 0;
                
                // Prevent drawing beyond the right edge
                if( (i == maxXTile - 1) && xOverDraw) {
                    wDest = rc.right - (i * bmpHeight);
                }


            }

            yDest += bmpHeight - srcY;
            srcY = 0;
            xDest = 0;

            // Prevent drawing beyond the bottom edge
            if( (k == maxYTile - 1) && yOverDraw) {
                wDest = rc.bottom - (k * bmpHeight);
            }


        }


        /*
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
        */
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