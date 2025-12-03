#pragma once

#include <windows.h>
#include "subclassed_controls.h"
#include "window_meterics.h"

class EnterScoreWindow : public WndAsClass<EnterScoreWindow> {
    friend WndAsClass;

    public:
        EnterScoreWindow() : hWnd(0), parentHWnd(0), scoreIndex(0), name(0), windowMoving(true) {
            strCache[0] = NULL;
            strCache[1] = NULL;
        }

        ~EnterScoreWindow() {
            if(name) {
                free(name);
            }
        }

        bool createWindow(const HINSTANCE& hInst, const HWND& parent, const HMONITOR& parentMonitor, size_t& inScoreIndex);

        static bool registerSelf(const HINSTANCE& hInst);
        inline const HWND& getHandle() const { return hWnd; }

        inline size_t const getScoreIndex() const { return scoreIndex; }
        inline wchar_t* const getName() const { return name; }

    private:

        void onCreate();
        void onDPIChange(const float xDPI, const float yDPI);
        void onWindowMoved();
        void moveControls();
        LRESULT windowProc(const UINT& msg, const WPARAM wParam, const LPARAM lParam);
        
        static bool isRegistered;
        bool windowMoving;
        HMONITOR prevMonitor;
        HWND hWnd;
        HWND parentHWnd;
        HWND labelMessage1;
        HWND labelMessage2;
        HWND textYourName;
        HWND buttonOK;

        wchar_t* strCache[2];

        size_t scoreIndex;
        wchar_t* name;

        WindowMetrics metrics;

};