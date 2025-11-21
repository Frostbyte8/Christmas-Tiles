#pragma once

#include <windows.h>
#include "subclassed_controls.h"
#include "window_meterics.h"
#include "../../model/scores.h"

class HighscoresWindow : public WndAsClass<HighscoresWindow> {
    friend WndAsClass;

    public:
        HighscoresWindow() : hWnd(0), parentHWnd(0) {}

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

        HWND labelCaption;
        
        HWND labelHeader[3]; // Name / Score / Date
        
        HWND labelName[ScoreTableConstants::MAX_SCORES];
        HWND labelScore[ScoreTableConstants::MAX_SCORES];
        HWND labelDate[ScoreTableConstants::MAX_SCORES];
        
        
        HWND buttonOK;

        WindowMetrics metrics;

};