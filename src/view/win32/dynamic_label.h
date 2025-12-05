#pragma once

#include <windows.h>
#include "subclassed_controls.h"

// Unlike your typical "Static" label, Dynamic labels are meant to have their text
// changed.

class DynamicLabel : public WndAsClass<DynamicLabel> {
    friend WndAsClass;

    public:
        DynamicLabel() : hWnd(0), caption(NULL), currentFont(0) {
        }
        ~DynamicLabel() {
            if(caption) {
                free(caption);
                caption = NULL;
            }
        }
        static bool registerSelf(HINSTANCE hInst);
        bool createWindow(const HINSTANCE& hInst, const wchar_t* inCaption, const HWND& parent, const HMENU& ID);
        void setText(const wchar_t* inCaption);
        UINT doLoop();
        inline const HWND getHandle() const { return hWnd; }

    private:
        void onPaint();
        wchar_t* caption;
        LRESULT windowProc(const UINT& msg, const WPARAM wParam, const LPARAM lParam);
        HWND hWnd;
        static bool isRegistered;
        HFONT currentFont;
};