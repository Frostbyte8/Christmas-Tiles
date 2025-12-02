#pragma once

#include <windows.h>
#include "subclassed_controls.h"
#include "window_meterics.h"
#include "../../model/scores.h"

class HighscoresWindow : public WndAsClass<HighscoresWindow> {
    friend WndAsClass;

    public:
        HighscoresWindow() : hWnd(0), parentHWnd(0), scoreTable(NULL), windowMoving(true) {}

        bool createWindow(const HINSTANCE& hInst, const HWND& parent, const ScoreTable& inScoreTable);

        static bool registerSelf(const HINSTANCE& hInst);
        inline const HWND& getHandle() const { return hWnd; }

    private:

        LONG findWidestName();

        void onCreate();
        void onDPIChange(const float xDPI, const float yDPI);
        void moveControls();
        void onWindowMoved();
        LRESULT windowProc(const UINT& msg, const WPARAM wParam, const LPARAM lParam);
        
        static bool isRegistered;
        bool windowMoving;
        HMONITOR prevMonitor;
        HWND hWnd;
        HWND parentHWnd;

        HWND labelCaption;
        
        HWND labelHeader[3]; // Name / Score / Date
        
        HWND labelName[ScoreTableConstants::MAX_SCORES];
        HWND labelScore[ScoreTableConstants::MAX_SCORES];
        HWND labelDate[ScoreTableConstants::MAX_SCORES];
        
        ScoreTable const *scoreTable;

        HWND buttonOK;

        WindowMetrics metrics;

};