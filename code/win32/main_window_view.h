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

class MainWindowView {
    
    public:
        MainWindowView() : window(NULL), tileset(NULL), gamePresenter(NULL) {}
        bool registerSelf(HINSTANCE hInstance);
        bool createWindow(HINSTANCE hInstance);
        UINT doLoop();
        static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    
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

        WORD gameXPos;
        HBRUSH gameBG;
        HBITMAP tileset;

        GamePresenter* gamePresenter;
        bool createControls();
        void moveControls();
        void onWindowMoved();
        void centerWindow();
        void onPaint();
        void onClick(const WPARAM& wParam, const LPARAM& lParam);
};

#endif // __MAIN_WINDOW_VIEW_H__
