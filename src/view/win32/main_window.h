#pragma once

#include <windows.h>
#include "subclassed_controls.h"
#include "game_panel.h"
#include "dynamic_label.h"
#include "window_meterics.h"

class MainWindowView : public WndAsClass<MainWindowView> {
    friend WndAsClass;

    public:

        // TODO: These should not be public :/
        MainWindowView(const HINSTANCE hIn) : hWnd(NULL), hInstance(hIn) {}
        bool registerSelf();
        bool createWindow();
        bool onCreate();
        UINT doLoop();
        void onSize(const WORD& width, const WORD& height);
        void onTimer();

    private:

        void onTileSelected(const WORD& tileIndex);
        void centerWindow();
        void moveControls();

        LONG getTallestPoint() const;
        LONG getWidestGroupBox() const;

        LRESULT windowProc(const UINT& msg, const WPARAM wParam, const LPARAM lParam);

        // This is a helpful function that will set the font on all the controls
        // to what the user has defined on their system.
        static bool CALLBACK SetProperFont(HWND child, LPARAM font) {
            ::SendMessage(child, WM_SETFONT, font, TRUE);
            return true;
        }

        // Disable copy constructor
        MainWindowView &operator=(const MainWindowView&);

        HWND hWnd;
        HMONITOR prevMonitor;

        HWND groupStats[3];
        HWND buttonPause;
                
        DynamicLabel scoreLabel;
        DynamicLabel pointsLabel;
        DynamicLabel timeLabel;

        GamePanel gamePanel;
        const HINSTANCE hInstance;
        WindowMetrics metrics;
        DWORD startTime;
        DWORD elapsedTime;
};