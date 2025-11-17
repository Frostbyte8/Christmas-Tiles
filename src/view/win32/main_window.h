#pragma once

#include <windows.h>
#include "subclassed_controls.h"
#include "game_panel.h"
#include "dynamic_label.h"
#include "window_meterics.h"
#include "../../presenter//main_window_presenter.h"
#include "../../interface/main_window_interface.h"

class MainWindowView : public WndAsClass<MainWindowView>, public MainWindowInterface {
    friend WndAsClass;

    public:

        // TODO: These should not be public :/
        MainWindowView(const HINSTANCE hIn) : hWnd(NULL), hInstance(hIn), shouldUnflip(false) {
            windowPresenter.setMainWindow(reinterpret_cast<MainWindowInterface*>(this));
        }

        bool createWindow();
        UINT doLoop();
        bool registerSelf();

        bool onCreate();
        // Interface Functions
        void implDisplayTestMessage();


    private:

        void centerWindow();
        LONG getTallestPoint() const;
        LONG getWidestGroupBox() const;
        void moveControls();
        void onTimer();
        void onElapsedTimeTimer();
        void onTileSelected(const WPARAM& wParam, const LPARAM& lParam);
        LRESULT windowProc(const UINT& msg, const WPARAM wParam, const LPARAM lParam);

        // This is a helpful function that will set the font on all the controls
        // to what the user has defined on their system.
        static bool CALLBACK SetProperFont(HWND child, LPARAM font) {
            ::SendMessage(child, WM_SETFONT, font, TRUE);
            return true;
        }

        // Disable copy constructor
        MainWindowView &operator=(const MainWindowView&);

        MainWindowPresenter windowPresenter;
        bool shouldUnflip;

        HWND hWnd;
        HMONITOR prevMonitor;

        HWND groupStats[3];
        HWND buttonPause;
                
        DynamicLabel scoreLabel;
        DynamicLabel pointsLabel;
        DynamicLabel timeLabel;

        GamePanel       gamePanel;
        const HINSTANCE hInstance;
        WindowMetrics   metrics;

};