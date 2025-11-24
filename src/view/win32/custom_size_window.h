#pragma once

#include <windows.h>
#include "subclassed_controls.h"
#include "window_meterics.h"


class CustomSizeWindow : public WndAsClass<CustomSizeWindow> {
    friend WndAsClass;

    public:

        CustomSizeWindow() : hWnd(0), parentHWnd(0) {
        }

        void onCreate();
        void moveControls();
        bool createWindow(const HINSTANCE& hInst, const HWND& parent);

        static bool registerSelf(const HINSTANCE& hInst);
        inline const HWND& getHandle() const { return hWnd; }

    private:
        LRESULT windowProc(const UINT& msg, const WPARAM wParam, const LPARAM lParam);
        void onWindowMoved();

        static bool isRegistered;
        HWND hWnd;
        HWND parentHWnd;
        
        HWND labelEnterSize;
        HWND labelCoord[2];
        HWND textCoord[2];
        HWND buttonOK;
        HWND buttonCancel;
        
        HMONITOR prevMonitor;
        WindowMetrics  metrics;
};