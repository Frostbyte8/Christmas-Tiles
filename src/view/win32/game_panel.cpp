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


    //if(!changeTileset(windowPresenter->getMainWindowData().pathToTileset)) {
    //    return false;
    //}

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

//==============================================================================
// Public Functions
//==============================================================================

//------------------------------------------------------------------------------
// changeTileset - Attempts to change the tileset to the new one provided.
//------------------------------------------------------------------------------

bool GamePanel::changeTileset(const wchar_t* fullPath) {

    HBITMAP tempBMP;
    tempBMP = (HBITMAP)LoadImage(NULL, fullPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    if(!tempBMP) {
        return false;
    }

    if(tilesetBMP) {
        DeleteObject(tilesetBMP);
    }

    tilesetBMP = tempBMP;

    BITMAP bmInfo = {0};
    GetObject(tilesetBMP, sizeof(BITMAP), &bmInfo);
    bmpWidth    = bmInfo.bmWidth;
    bmpHeight   = bmInfo.bmHeight;

    return true;
}

//------------------------------------------------------------------------------
// updateVirtualSize - Update how big the Gameboard really is compared to how
// much is visible.
//------------------------------------------------------------------------------

void GamePanel::updateVirtualSize(const WORD& newXTiles, const WORD& newYTiles) {
    virtualWidth = static_cast<WORD>(newXTiles * bmpHeight);
    virtualHeight = static_cast<WORD>(newYTiles * bmpHeight);
    xOffset = 0;
    yOffset = 0;
    SetScrollPos(hWnd, SB_HORZ, 0, TRUE);
    SetScrollPos(hWnd, SB_VERT, 0, TRUE);
    onSize();
}

//==============================================================================
// Window Messages
//==============================================================================

//------------------------------------------------------------------------------
// onHScroll - When the user scrolls the the game board horizontally
//------------------------------------------------------------------------------

void GamePanel::onHScroll(const WORD& dir, const WORD& pos) {

    int minRange = 0;
    int maxRange = 0;

    GetScrollRange(hWnd, SB_HORZ, &minRange, &maxRange);

    if(dir == SB_LINERIGHT) {

        xOffset += bmpHeight / 8;
        
        if(xOffset > maxRange) {
            xOffset = maxRange;
        }

        SetScrollPos(hWnd, SB_HORZ, xOffset, TRUE);
    }
    else if(dir == SB_PAGERIGHT) {
        xOffset += bmpHeight / 4;
        
        if(xOffset > maxRange) {
            xOffset = maxRange;
        }
        SetScrollPos(hWnd, SB_HORZ, xOffset, TRUE);
    }
    else if(dir == SB_LINELEFT) {

        xOffset -= bmpHeight / 8;

        if(xOffset < 0) {
            xOffset = 0;
        }
        SetScrollPos(hWnd, SB_HORZ, xOffset, TRUE);

    }
    else if(dir == SB_PAGELEFT) {
        xOffset -= bmpHeight / 4;

        if(xOffset < 0) {
            xOffset = 0;
        }
        SetScrollPos(hWnd, SB_HORZ, xOffset, TRUE);
    }
    else if(dir == SB_THUMBPOSITION || dir == SB_THUMBTRACK) {
        xOffset = pos;
        SetScrollPos(hWnd, SB_HORZ, xOffset, TRUE);
    }
    else {
        return;
    }
    InvalidateRect(hWnd, NULL, TRUE);

}

//------------------------------------------------------------------------------
// onVScroll - When the user scrolls the the game board vertically
//------------------------------------------------------------------------------

void GamePanel::onVScroll(const WORD& dir, const WORD& pos) {

    int minRange = 0;
    int maxRange = 0;

    GetScrollRange(hWnd, SB_VERT, &minRange, &maxRange);

    if(dir == SB_LINEDOWN) {

        yOffset += bmpHeight / 8;
        
        if(yOffset > maxRange) {
            yOffset = maxRange;
        }

        SetScrollPos(hWnd, SB_VERT, yOffset, TRUE);
    }
    else if(dir == SB_PAGEDOWN) {
        yOffset += bmpHeight / 4;
        
        if(yOffset > maxRange) {
            yOffset = maxRange;
        }
        SetScrollPos(hWnd, SB_VERT, yOffset, TRUE);
    }
    else if(dir == SB_LINEUP) {

        yOffset -= bmpHeight / 8;

        if(yOffset < 0) {
            yOffset = 0;
        }
        SetScrollPos(hWnd, SB_VERT, yOffset, TRUE);

    }
    else if(dir == SB_PAGEUP) {
        yOffset -= bmpHeight / 4;

        if(yOffset < 0) {
            yOffset = 0;
        }
        SetScrollPos(hWnd, SB_VERT, yOffset, TRUE);
    }
    else if(dir == SB_THUMBPOSITION || dir == SB_THUMBTRACK) {
        yOffset = pos;
        SetScrollPos(hWnd, SB_VERT, yOffset, TRUE);
    }
    else {
        return;
    }

    InvalidateRect(hWnd, NULL, TRUE);

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
        return;
    }

    HDC         backBuffer;
    HBITMAP     backBitmap, prevBitmap;
    HDC         tilesetHDC;

    // Create a back buffer
    // TODO: Only need to recreate after WM_DISPLAYCHANGE, and maybe WM_FONTCHANGE, as well as when the RC doesn't match
    // Also we only need to draw to the screen, that which has been invalidated, and we don't need
    // to erase the background every time
    createBackBuffer(ps.hdc, backBuffer, backBitmap, prevBitmap, rc);
    
    FillRect(backBuffer, &rc, (HBRUSH)GetClassLongPtr(hWnd, GCLP_HBRBACKGROUND));
    
    tilesetHDC = CreateCompatibleDC(ps.hdc);
    HBITMAP tempBMP = (HBITMAP)SelectObject(tilesetHDC, tilesetBMP);

    if(windowPresenter) {

        const GameBoard& gameBoard = windowPresenter->getGameBoard();
        const GameBoardDimensions& boardDimensions = gameBoard.getBoardDimensions();
        const std::vector<TileData>& tiles = gameBoard.getTiles();

        unsigned const int minXTile = static_cast<int>(floor(static_cast<float>(xOffset) / bmpHeight));
        unsigned const int minYTile = static_cast<int>(floor(static_cast<float>(yOffset) / bmpHeight));

        unsigned int maxXTile = minXTile + static_cast<int>(ceil(static_cast<float>(rc.right + xOffset) / bmpHeight));
        unsigned int maxYTile = minYTile + static_cast<int>(ceil(static_cast<float>(rc.bottom + yOffset) / bmpHeight));

        if(maxXTile > boardDimensions.width) {
            maxXTile = boardDimensions.width;
        }

        if(maxYTile >  boardDimensions.height) {
            maxYTile = boardDimensions.height;
        }
        
        int xDest = 0;
        int yDest = 0;
        int wDest = bmpHeight;
        int hDest = bmpHeight;
        int srcY = (yOffset % bmpHeight);

        // Check and see if the last tile will go over the edge of the back buffer
        const bool xOverDraw = (bmpHeight - (xOffset % bmpHeight)) + ((maxXTile - (minXTile + 1)) * bmpHeight) > static_cast<unsigned long>(rc.right) ? true : false;
        const bool yOverDraw = (bmpHeight - (yOffset % bmpHeight)) + ((maxYTile - (minYTile + 1)) * bmpHeight) > static_cast<unsigned long>(rc.bottom) ? true : false;

        for(unsigned int k = minYTile; k < maxYTile; ++k) {

            int srcX = (xOffset % bmpHeight);
            wDest = bmpHeight;

            for(unsigned int i = minXTile; i < maxXTile; ++i) {

                // Note to self: MSVC does in fact optimize this!
                const int realIndex = DX2_TO_DX1(i, k, boardDimensions.width);
                
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

//------------------------------------------------------------------------------
// onSize - Resizes the panel and adjusts things on it as needed.
//------------------------------------------------------------------------------

void GamePanel::onSize() {

    RECT rc;
    ShowScrollBar(hWnd, SB_BOTH, FALSE); // Turn off scrollbars to get correct client size.
    GetClientRect(hWnd, &rc);

    if(rc.right == 0 || rc.bottom == 0) {
        return;
        // In the future, run an assert.
    }

    // First, we need to figure out which scroll bars we need

    const int ScrollSpanX = GetSystemMetrics(SM_CXHSCROLL);
    const int ScrollSpanY = GetSystemMetrics(SM_CXVSCROLL);

    int whichBars = 0;

    if(rc.right < virtualWidth) {

        // We might also need the other scrollbar if the span of the scrollbar
        // makes the space small enough

        if(rc.bottom - ScrollSpanX < virtualHeight) {
            ShowScrollBar(hWnd, SB_BOTH, TRUE);
            whichBars = ScrollType::BOTH;
            rc.right -= ScrollSpanY;
            rc.bottom -= ScrollSpanX;
         }
        else {
            ShowScrollBar(hWnd, SB_HORZ, TRUE);
            ShowScrollBar(hWnd, SB_VERT, FALSE);
            whichBars = ScrollType::HORZ;
            rc.bottom -= ScrollSpanX;
        }

    }
    else if(rc.bottom < virtualHeight) {
        
        if(rc.right - ScrollSpanY < virtualWidth) {
            ShowScrollBar(hWnd, SB_BOTH, TRUE);
            whichBars = ScrollType::BOTH;
            rc.right -= ScrollSpanY;
            rc.bottom -= ScrollSpanX;
        }
        else {
            ShowScrollBar(hWnd, SB_VERT, TRUE);
            ShowScrollBar(hWnd, SB_HORZ, FALSE);
            whichBars = ScrollType::VERT;
            rc.right -= ScrollSpanY;
        }

    }
    else {
        ShowScrollBar(hWnd, SB_BOTH, FALSE);
        return;
    }

    if(whichBars & ScrollType::HORZ) {
        SetScrollRange(hWnd, SB_HORZ, 0, virtualWidth - rc.right, TRUE);
    }

    if(whichBars & ScrollType::VERT) {
        SetScrollRange(hWnd, SB_VERT, 0, virtualHeight - rc.bottom, TRUE);
    }

}



//==============================================================================
// Private Functions
//==============================================================================

//------------------------------------------------------------------------------
// windowProc - Standard window procedure for a window
//------------------------------------------------------------------------------

LRESULT GamePanel::windowProc(const UINT& msg, const WPARAM wParam, const LPARAM lParam) {

    switch(msg) {
        
        default: return DefWindowProc(hWnd, msg, wParam, lParam);
        case WM_HSCROLL: onHScroll(LOWORD(wParam), HIWORD(wParam)); break;
        case WM_VSCROLL: onVScroll(LOWORD(wParam), HIWORD(wParam)); break;
        case WM_LBUTTONDOWN: SendMessage(GetParent(hWnd), UWM_TILE_SELECTED, LOWORD(lParam) + xOffset, HIWORD(lParam) + yOffset); break;
        case WM_PAINT:      onPaint(); break;
        case WM_ERASEBKGND: return -1; break; // We will redraw everything in WM_PAINT
        case WM_CLOSE:      DestroyWindow(hWnd); break;
    }
    return 0;
}