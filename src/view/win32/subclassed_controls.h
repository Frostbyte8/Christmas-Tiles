#pragma once

#include <windows.h>

// WPARAM = Tile Index Selected
// I'm starting with 100 here, in case I need the first 100 ever for anything.
// TODO: Move this somewhere better
#define UWM_TILE_SELECTED   (WM_USER + 100)
#define UWM_SCORE_ENTERED   (WM_USER + 101)

// This makes it so Windows can be put inside a class.

template <class T>
class WndAsClass {
    public:
        static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
            T* self;

            // TODO: Look into options to avoid duplicating this code.

            if(msg == WM_NCCREATE) {
                // Store a copy of an instance of this window in the USERDATA section
                self = static_cast<T*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
                self->hWnd = hWnd;

                SetLastError(0);

                if (!SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self))) {
                    const DWORD ERR = GetLastError();
                    if(ERR != 0) {
                        return FALSE;
                    }
                }
            }
            else {
                self = reinterpret_cast<T*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
            }

            if(self) {
                return self->windowProc(msg, wParam, lParam);
            }

            return DefWindowProc(hWnd, msg, wParam, lParam);

        }
};