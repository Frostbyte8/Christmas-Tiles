#ifndef __MAIN_WINDOW_VIEW_H__
#define __MAIN_WINDOW_VIEW_H__

#include <windows.h>

namespace ControlIDs {
    enum ControlIDs {
        GRP_SCORE = 0,
        GRP_POINTS,
        GRP_TIME,
        BTN_PAUSE,
        LBL_SCORE,
        LBL_POINTS,
        LBL_GAMEBOARD,
        COUNT,
    };
}

class MainWindowView {
    
    public:
        MainWindowView() : window(NULL) {}
        bool registerSelf(HINSTANCE hInstance);
        bool createWindow(HINSTANCE hInstance);
        UINT doLoop();
        static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    
    private:
        HWND window;
        HWND controls[ControlIDs::COUNT];
        HMONITOR prevMonitor;
        LRESULT windowProc(const UINT& msg, const WPARAM wParam, const LPARAM lParam);

        bool createControls();
        void onWindowMoved();
        void centerWindow();
};

#endif // __MAIN_WINDOW_VIEW_H__
