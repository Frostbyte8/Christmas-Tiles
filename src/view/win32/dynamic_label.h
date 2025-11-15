#pragma once

#include <windows.h>
#include "subclassed_controls.h"

// Unlike your typical "Static" label, Dynamic labels are meant to have their text
// changed.

class DynamicLabel : public WndAsClass<DynamicLabel> {
    friend WndAsClass;

    public:
        DynamicLabel() : hWnd(0) {}
        static bool registerSelf(HINSTANCE hInst); // TODO: Make this static?
        bool createWindow(const HINSTANCE& hInst, wchar_t inCaption[], const HWND& parent, const HMENU& ID);
        UINT doLoop();
        inline const HWND getHandle() const { return hWnd; }

    private:
        void onPaint();
        LRESULT windowProc(const UINT& msg, const WPARAM wParam, const LPARAM lParam);
        HWND hWnd;
        static bool isRegistered;
        HFONT currentFont;
};