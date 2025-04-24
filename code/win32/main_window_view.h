#ifndef __MAIN_WINDOW_VIEW_H__
#define __MAIN_WINDOW_VIEW_H__

#include <windows.h>
#include "window_metrics.h"
#include "..\presenter\game_presenter.h"
#include "gameboard_panel.h"

namespace ControlIDs {
    enum ControlIDs {
        GRP_SCORE = 0,
        GRP_POINTS,
        GRP_TIME,
        BTN_PAUSE,
        LBL_SCORE,
        LBL_POINTS,
        LBL_TIME,
        VIEW_GAMEBOARD,
        COUNT,
    };
}

namespace TimerIDs {
    enum TimerIDs {
        UNFLIP_TIMER = 1,
        UPDATE_TIMER = 2,
    };
}

namespace MenuIDs {
    enum MenuIDs {

        NEW_GAME     = 401,
        PAUSE_GAME   = 402,
        HIGHSCORES   = 403,
        EXIT         = 404,

        TILESET      = 410,
        BOARD_3X3    = 411,
        BOARD_4X4    = 412,
        BOARD_5X5    = 413,
        BOARD_5X9    = 414,
        BOARD_10x10  = 415,
        BOARD_CUSTOM = 416,

        HELP_FILE    = 421,
        ABOUT        = 422,
    };
}

class MainWindowView : public MainWindowInterface, WndAsClass<MainWindowView> {
    friend WndAsClass; 

    public:
        MainWindowView() : window(NULL), gamePresenter(NULL) {}
        bool registerSelf(HINSTANCE hInstance);
        bool createWindow(HINSTANCE hInstance);
        UINT doLoop();
        //static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

        // Interface Functions
        virtual void gameWon();
        virtual int askQuestion(const std::string& message, const std::string& title, const int& mbType);

    private:

        HWND controls[ControlIDs::COUNT];
        HMONITOR prevMonitor;
        LRESULT windowProc(const UINT& msg, const WPARAM wParam, const LPARAM lParam);

        static bool CALLBACK SetProperFont(HWND child, LPARAM font) {
            ::SendMessage(child, WM_SETFONT, font, true);
            return true;
        }

        HWND window;
        WindowMetrics wm;

        HMENU menuBar;
        HMENU fileMenu;
        HMENU optionsMenu;
        HMENU helpMenu;

        //WNDPROC prevLabelProcs[3];
        //HBRUSH gameBG;

        HBITMAP tileset;
        GameBoardPanel gameBoardPanel;

        GamePresenter* gamePresenter;
        void createMenubar();
        bool createControls();
        void moveControls();
        void centerWindow();
        void updateLabels();
        void onSelectMenuItem(const WORD& itemID);
        void newGame(const bool forceNew);
        void onTogglePause();

        LRESULT onGameBoardClick(const WPARAM& wParam, const LPARAM& lParam);
        LRESULT onPaint();
        LRESULT onTimer(const UINT& timerID);
        LRESULT onWindowMoved();

};


#endif // __MAIN_WINDOW_VIEW_H__

