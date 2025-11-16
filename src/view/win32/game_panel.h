#pragma once

#include <windows.h>
#include "subclassed_controls.h"
#include "../../model/gameboard.h"
#include "../../presenter/main_window_presenter.h"

// TODO: MainWindowInterface

class GamePanel : public WndAsClass<GamePanel> {
    friend WndAsClass;

    public:
        GamePanel(): hWnd(NULL), tilesetBMP(0), windowPresenter(NULL) {}
        static bool registerSelf(HINSTANCE hInst); // TODO: Make this static?
        bool createWindow(const HINSTANCE& hInst, const HWND& parent, const HMENU& ID);
        UINT doLoop();

        inline const HWND& getHandle() const { return hWnd; }
        void setWindowPresenter(MainWindowPresenter* mwp) {
            windowPresenter = mwp;
        }

        

    private:
        void onClick(WORD xPos, WORD yPos);
        void onPaint();
        LRESULT windowProc(const UINT& msg, const WPARAM wParam, const LPARAM lParam);

        MainWindowPresenter const *windowPresenter;

        HWND hWnd;
        static bool isRegistered;
        
        HBITMAP tilesetBMP;
};