#include "dynamic_label.h"
#include "subclassed_controls.h"
#include "shared_functions.h"

bool DynamicLabel::isRegistered = false;

const static int MAX_CAPTION_SIZE = 16;

//-----------------------------------------------------------------------------
// registerSelf - Registers the Window class.
//-----------------------------------------------------------------------------

bool DynamicLabel::registerSelf(HINSTANCE hInst) {
    
    if(isRegistered) {
        MessageBox(NULL, L"Control 'DynamicLabel' is already registered!", L"Already Registered", MB_OK);
        return false;
    }

    WNDCLASSEX wc;

    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = DynamicLabel::WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInst;
    wc.hIcon         = NULL;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = L"DynamicLabel";
    wc.hIconSm       = NULL;

    if(!RegisterClassEx(&wc)) {
        MessageBox(NULL, L"Error registering window class.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    isRegistered = true;

    return true;
}

//-----------------------------------------------------------------------------
// createWindow - Creates the main Window.
//-----------------------------------------------------------------------------

bool DynamicLabel::createWindow(const HINSTANCE& hInst, const wchar_t* inCaption, const HWND& parent, const HMENU& ID) {

    if(hWnd) {
        return true; // Already created.
    }

    hWnd = CreateWindowEx(0, L"DynamicLabel", inCaption,
        WS_CHILD | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 0, 0,
        parent, ID, hInst, this);

    if(hWnd == NULL) {
        MessageBox(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return false;
    }

    ShowWindow(hWnd, SW_NORMAL);
    UpdateWindow(hWnd);

    return true;
}

void DynamicLabel::setText(const wchar_t* inCaption) {

    // It's safe to store our own string, so long as we follow what is
    // outlined by Raymond Chen's Post on 2003-08-21.

    if(!inCaption) {

        // Null inCaption means no string at all.

        if(caption) {
            free(caption);
            caption = NULL;
        }

        return;
    }

    const size_t strLen = wcsnlen_s(inCaption, MAX_CAPTION_SIZE);
    
    // We will use the very first wchar_t to store the contents of our size_t.
    // So kind of like a Fatty Struct/Pointer.

    wchar_t* newCaption = (wchar_t*)malloc((sizeof(wchar_t) * (strLen+2)));

    if(!newCaption) {
        return;
    }

    newCaption[strLen] = 0;
    newCaption[0] = static_cast<wchar_t>(strLen);

    if(strLen != 0) {
        wcscpy_s(&newCaption[1], strLen+1, inCaption);
    }

    if(caption) {
        free(caption);
        caption = NULL;
    }

    caption = newCaption;
    
}

//-----------------------------------------------------------------------------
// windowProc - Standard window procedure for a window
//-----------------------------------------------------------------------------

LRESULT DynamicLabel::windowProc(const UINT& msg, const WPARAM wParam, const LPARAM lParam) {

    switch(msg) {
        
        default: return DefWindowProc(hWnd, msg, wParam, lParam);
        case WM_PAINT:      onPaint(); break;
        case WM_ERASEBKGND: return -1; break; // We will redraw everything in WM_PAINT
        case WM_CLOSE:      DestroyWindow(hWnd); break;
        
        case WM_CREATE:
            setText( (reinterpret_cast<CREATESTRUCT*>(lParam))->lpszName);
            break;
        case WM_SETTEXT:
            setText(reinterpret_cast<wchar_t*>(lParam));
            InvalidateRect(hWnd, NULL, FALSE);
            break;

        case WM_SETFONT: 
            currentFont = (HFONT)wParam;
            InvalidateRect(hWnd, NULL, FALSE);
            break;

    }
    return 0;

}

//-----------------------------------------------------------------------------
// onPaint - Processes the WM_PAINT message.
//-----------------------------------------------------------------------------

void DynamicLabel::onPaint() {

    RECT rc;
    GetClientRect(hWnd, &rc);

    PAINTSTRUCT ps;
    if(!BeginPaint(hWnd, &ps)) {
        // Painting Failed
        return;
    }

    HDC         backBuffer;
    HBITMAP     backBitmap, prevBitmap;

    createBackBuffer(ps.hdc, backBuffer, backBitmap, prevBitmap, rc);
    
    // Begin Drawing
    FillRect(backBuffer, &rc, (HBRUSH)GetClassLongPtr(hWnd, GCLP_HBRBACKGROUND));

    SetBkMode(backBuffer, TRANSPARENT);

    if(caption) {

        const int strLen = static_cast<int>(caption[0]);

        if(strLen != 0) {

            HFONT oldFont = (HFONT)SelectObject(backBuffer, currentFont);
            SetTextColor(backBuffer, GetSysColor(COLOR_WINDOWTEXT));
            DrawText(backBuffer, &caption[1], strLen, &rc, DT_CENTER);
            SelectObject(backBuffer, oldFont);

        }
    }

    // Finalize drawing by Copying back buffer to DC
    BitBlt(ps.hdc, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, backBuffer, 0, 0, SRCCOPY);

    // Destroy BackBuffer
    destroyBackBuffer(backBuffer, backBitmap, prevBitmap);

    EndPaint(hWnd, &ps);

}