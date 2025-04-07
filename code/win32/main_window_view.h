#ifndef __MAIN_WINDOW_VIEW_H__
#define __MAIN_WINDOW_VIEW_H__

#include <windows.h>
#include "window_metrics.h"
#include "..\presenter\game_presenter.h"

namespace ControlIDs {
    enum ControlIDs {
        GRP_SCORE = 0,
        GRP_POINTS,
        GRP_TIME,
        BTN_PAUSE,
        LBL_SCORE,
        LBL_POINTS,
        LBL_TIME,
        COUNT,
    };
}

namespace TimerIDs {
    enum TimerIDs {
        UNFLIP_TIMER = 1,
        UPDATE_TIMER = 2,
    };
}

class MainWindowView : public MainWindowInterface {
    
    public:
        MainWindowView() : window(NULL), tileset(NULL), gamePresenter(NULL) {}
        bool registerSelf(HINSTANCE hInstance);
        bool createWindow(HINSTANCE hInstance);
        UINT doLoop();
        static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

        // Interface Functions
        virtual void gameWon();
        int askQuestion(const std::string& message, const std::string& title, const int& mbType);

    private:

        HWND window;
        HWND controls[ControlIDs::COUNT];
        HMONITOR prevMonitor;
        LRESULT windowProc(const UINT& msg, const WPARAM wParam, const LPARAM lParam);

        static bool CALLBACK SetProperFont(HWND child, LPARAM font) {
            ::SendMessage(child, WM_SETFONT, font, true);
            return true;
        }


        WindowMetrics wm;

        //WNDPROC prevLabelProcs[3];

        WORD gameXPos;
        HBRUSH gameBG;
        HBITMAP tileset;

        GamePresenter* gamePresenter;
        bool createControls();
        void moveControls();
        void centerWindow();

        LRESULT onClick(const WPARAM& wParam, const LPARAM& lParam);
        LRESULT onPaint();
        LRESULT onTimer(const UINT& timerID);
        LRESULT onWindowMoved();

};

#endif // __MAIN_WINDOW_VIEW_H__

