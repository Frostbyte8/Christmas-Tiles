#pragma once

#include <windows.h>
#include "subclassed_controls.h"
#include "window_meterics.h"
#include "../../stdint_compat.h"

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
        inline const uint8_t& getNewWidth() const { return newWidth; } 
        inline const uint8_t& getNewHeight() const { return newHeight; }

    private:
        LRESULT windowProc(const UINT& msg, const WPARAM wParam, const LPARAM lParam);
        void onWindowMoved();
        void saveData();

        static bool isRegistered;
        HWND hWnd;
        HWND parentHWnd;
        
        HWND labelEnterSize;
        HWND labelCoord[2];
        HWND textCoord[2];
        HWND buttonOK;
        HWND buttonCancel;

        uint8_t newWidth;
        uint8_t newHeight;
        
        HMONITOR prevMonitor;
        WindowMetrics  metrics;
};