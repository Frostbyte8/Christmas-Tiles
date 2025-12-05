#pragma once

#include <windows.h>
#include "subclassed_controls.h"
#include "window_meterics.h"
#include "../../stdint_compat.h"

class CustomSizeWindow : public WndAsClass<CustomSizeWindow> {
    friend WndAsClass;

    public:

        CustomSizeWindow() : hWnd(0), parentHWnd(0), windowMoving(true), buttonCancel(NULL), buttonOK(NULL),
        labelEnterSize(NULL), newWidth(0), newHeight(0), prevMonitor(NULL) {
            labelCoord[0] = NULL;
            labelCoord[1] = NULL;
            textCoord[0] = NULL;
            textCoord[1] = NULL;
        }

        bool createWindow(const HINSTANCE& hInst, const HWND& parent, const HMONITOR& parentMonitor);

        static bool registerSelf(const HINSTANCE& hInst);
        inline const HWND& getHandle() const { return hWnd; }
        inline const unsigned int& getNewWidth() const { return newWidth; } 
        inline const unsigned int& getNewHeight() const { return newHeight; }

    private:
        LRESULT windowProc(const UINT& msg, const WPARAM wParam, const LPARAM lParam);
        void onCreate();
        void onDPIChange(const float xDPI, const float yDPI);
        void moveControls();
        void onWindowMoved();
        void saveData();

        static bool isRegistered;
        bool windowMoving;
        HWND hWnd;
        HWND parentHWnd;
        
        HWND labelEnterSize;
        HWND labelCoord[2];
        HWND textCoord[2];
        HWND buttonOK;
        HWND buttonCancel;

        unsigned int newWidth;
        unsigned int newHeight;
        
        HMONITOR prevMonitor;
        WindowMetrics  metrics;
};