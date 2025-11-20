#pragma once

#include <windows.h>
#include "subclassed_controls.h"
#include "window_meterics.h"

class EnterScoreWindow : public WndAsClass<EnterScoreWindow> {
    friend WndAsClass;

    public:
        EnterScoreWindow() : hWnd(0), parentHWnd(0) {
        }

        bool onCreate();
        void moveControls();
        bool createWindow(const HINSTANCE& hInst, const HWND& parent);

        static bool registerSelf(const HINSTANCE& hInst);
        inline const HWND& getHandle() const { return hWnd; }

    private:

        LRESULT windowProc(const UINT& msg, const WPARAM wParam, const LPARAM lParam);
        static bool CALLBACK SetProperFont(HWND child, LPARAM font) {
            ::SendMessage(child, WM_SETFONT, font, TRUE);
            return true;
        }
        
        static bool isRegistered;
        HWND hWnd;
        HWND parentHWnd;
        

};