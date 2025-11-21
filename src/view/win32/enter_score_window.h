#pragma once

#include <windows.h>
#include "subclassed_controls.h"
#include "window_meterics.h"

class EnterScoreWindow : public WndAsClass<EnterScoreWindow> {
    friend WndAsClass;

    public:
        EnterScoreWindow() : hWnd(0), parentHWnd(0) {
            strCache[0] = NULL;
            strCache[1] = NULL;
        }

        bool createWindow(const HINSTANCE& hInst, const HWND& parent);

        static bool registerSelf(const HINSTANCE& hInst);
        inline const HWND& getHandle() const { return hWnd; }

    private:

        void onCreate();
        void moveControls();
        LRESULT windowProc(const UINT& msg, const WPARAM wParam, const LPARAM lParam);
        
        static bool isRegistered;
        HWND hWnd;
        HWND parentHWnd;
        HWND labelMessage1;
        HWND labelMessage2;
        HWND textYourName;
        HWND buttonOK;

        wchar_t* strCache[2];

        WindowMetrics metrics;

};