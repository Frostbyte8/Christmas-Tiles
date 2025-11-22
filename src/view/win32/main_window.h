#pragma once

#include <windows.h>
#include "subclassed_controls.h"
#include "game_panel.h"
#include "dynamic_label.h"
#include "window_meterics.h"
#include "../../presenter/main_window_presenter.h"
#include "../../interface/main_window_interface.h"

#include "about_window.h"
#include "enter_score_window.h"
#include "highscores_window.h"

class MainWindowView : public WndAsClass<MainWindowView>, public MainWindowInterface {
    friend WndAsClass;

    public:

        // TODO: These should not be public :/
        MainWindowView(const HINSTANCE hIn) : hWnd(NULL), hInstance(hIn), shouldUnflip(false) {
            windowPresenter.setMainWindow(reinterpret_cast<MainWindowInterface*>(this));
        }

        __forceinline void createMenuBar();
        bool createWindow();
        UINT doLoop();
        bool registerSelf();

        bool onCreate();
        // Interface Functions
        int implAskYesNoQuestion(const wchar_t* message, const wchar_t* title);
        void implGameStateChanged(const int& gameState);


    private:

        void centerWindow();
        LONG getTallestPoint() const;
        LONG getWidestGroupBox() const;
        void moveControls();
        void onClose();
        void onChangeBoardSize(const int& menuID);
        void onTimer();
        void onElapsedTimeTimer();
        void onTileSelected(const WPARAM& wParam, const LPARAM& lParam);
        LRESULT windowProc(const UINT& msg, const WPARAM wParam, const LPARAM lParam);

        // Disable copy constructor
        MainWindowView &operator=(const MainWindowView&);

        MainWindowPresenter windowPresenter;
        bool shouldUnflip;

        HWND hWnd;
        AboutWindow aboutWindow;
        EnterScoreWindow enterScoreWindow;
        HighscoresWindow highscoresWindow;
        ScoreTable scoreTable;
        
        HMENU menuBar;
        HMENU fileMenu;
        HMENU optionsMenu;
        HMENU helpMenu;

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