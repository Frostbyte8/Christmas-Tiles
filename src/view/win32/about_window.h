#pragma once

#include <windows.h>
#include "subclassed_controls.h"
#include "window_meterics.h"

class AboutWindow : public WndAsClass<AboutWindow> {
    friend WndAsClass;

    public:

        AboutWindow() : hWnd(0), parentHWnd(0), iconHandle(0) {
        }

        ~AboutWindow() {
            if(iconHandle) {
                DestroyIcon((HICON)iconHandle);
                iconHandle = NULL;
            }
        }

        bool onCreate();
        void moveControls();
        bool createWindow(const HINSTANCE& hInst, const HWND& parent);
        UINT doLoop();
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

        HANDLE iconHandle;
        HWND appIcon;
        HWND labelTitle;
        HWND labelVersion;
        HWND seperatorBar;
        HWND labelCopyright;
        HWND buttonOK;

        WindowMetrics  metrics;
};