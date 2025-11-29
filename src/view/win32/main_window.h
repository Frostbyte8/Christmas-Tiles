#pragma once

#include <windows.h>

#include "../../presenter/main_window_presenter.h"
#include "../../interface/main_window_interface.h"

#include "window_meterics.h"

#include "dynamic_label.h"
#include "game_panel.h"
#include "subclassed_controls.h"

#include "about_window.h"
#include "custom_size_window.h"
#include "enter_score_window.h"
#include "highscores_window.h"


class MainWindowView : public WndAsClass<MainWindowView>, public MainWindowInterface {
    friend WndAsClass;

    public:

        MainWindowView(const HINSTANCE hIn);

        bool createWindow();
        bool registerSelf();
        UINT doLoop();
        
        // Public interface functions

        int implAskYesNoQuestion(const wchar_t* message, const wchar_t* title);
        void implGameStateChanged(const int& gameState);

    private:
        
        // Disable Copy Constructor

        MainWindowView(const MainWindowView&);
        MainWindowView& operator = (const MainWindowView&);
        
        // Window Message Processing
        
        void onChangeBoardSize(const int& menuID);
        void onChangeTileset();
        void onClose();
        bool onCreate();
        void onElapsedTimeTimer();
        void onTileSelected(const WPARAM& wParam, const LPARAM& lParam);
        void onWindowMoved();

        LRESULT windowProc(const UINT& msg, const WPARAM wParam, const LPARAM lParam);

        // Private Functions

        __forceinline void createMenuBar();
        LONG getTallestPoint() const;
        LONG getWidestGroupBox() const;
        void moveControls();
        void updateBoardSizeMenu(const uint8_t& width, const uint8_t& height, bool clearChecks);

        // Member Variables
        
        MainWindowPresenter windowPresenter;

        HWND hWnd;
        HWND activeModalDialog;
        HMONITOR prevMonitor;
        const HINSTANCE hInstance;

        AboutWindow aboutWindow;
        EnterScoreWindow enterScoreWindow;
        HighscoresWindow highscoresWindow;
        CustomSizeWindow customSizeWindow;
        
        HMENU   menuBar;
        HMENU   fileMenu;
        HMENU   optionsMenu;
        HMENU   helpMenu;      

        HWND            groupStats[3];
        HWND            buttonPause;
        DynamicLabel    scoreLabel;
        DynamicLabel    pointsLabel;
        DynamicLabel    timeLabel;
        GamePanel       gamePanel;
        
        WindowMetrics   metrics;
        bool            shouldUnflip;

};