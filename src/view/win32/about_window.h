#pragma once

#include <windows.h>
#include "subclassed_controls.h"

class AboutWindow : public WndAsClass<AboutWindow> {
    friend WndAsClass;

    public:

        AboutWindow() : hWnd(0), parentHWnd(0) {}
        bool createWindow(const HINSTANCE& hInst, const HWND& parent);
        UINT doLoop();
        static bool registerSelf(const HINSTANCE& hInst);
        inline const HWND& getHandle() const { return hWnd; }

    private:
        LRESULT windowProc(const UINT& msg, const WPARAM wParam, const LPARAM lParam);
        
        static bool isRegistered;
        HWND hWnd;
        HWND parentHWnd;

};