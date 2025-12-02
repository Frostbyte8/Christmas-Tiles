#pragma once

#include <windows.h>
#include "subclassed_controls.h"
#include "window_meterics.h"

class AboutWindow : public WndAsClass<AboutWindow> {
    friend WndAsClass;

    public:

        AboutWindow() : hWnd(0), parentHWnd(0), iconHandle(0), windowMoving(true) {
        }

        ~AboutWindow() {
            if(iconHandle) {
                DestroyIcon((HICON)iconHandle);
                iconHandle = NULL;
            }
        }

        bool createWindow(const HINSTANCE& hInst, const HWND& parent);

        static bool registerSelf(const HINSTANCE& hInst);
        inline const HWND& getHandle() const { return hWnd; }

    private:
        LRESULT windowProc(const UINT& msg, const WPARAM wParam, const LPARAM lParam);
        bool onCreate();
        void onDPIChange(const float xDPI, const float yDPI);
        void onWindowMoved();
        void moveControls();

        static bool isRegistered;
        bool windowMoving;
        HWND hWnd;
        HWND parentHWnd;
        HMONITOR prevMonitor;

        HANDLE iconHandle;
        HWND appIcon;
        HWND labelTitle;
        HWND labelVersion;
        HWND seperatorBar;
        HWND labelCopyright;
        HWND buttonOK;

        WindowMetrics  metrics;
};