#pragma once

#include <windows.h>
#include "subclassed_controls.h"
#include "../../model/gameboard.h"

// TODO: MainWindowInterface

class GamePanel : public WndAsClass<GamePanel> {
    friend WndAsClass;

    public:
        GamePanel(): hWnd(NULL), tilesetBMP(0), shouldUnflip(true) {}
        static bool registerSelf(HINSTANCE hInst); // TODO: Make this static?
        bool createWindow(const HINSTANCE& hInst, const HWND& parent, const HMENU& ID);
        UINT doLoop();

        inline const HWND getHandle() const { return hWnd; }

    private:
        void onClick(WORD xPos, WORD yPos);
        void onPaint();
        LRESULT windowProc(const UINT& msg, const WPARAM wParam, const LPARAM lParam);

        HWND hWnd;
        static bool isRegistered;
        GameBoard gameBoard;
        HBITMAP tilesetBMP;
        bool shouldUnflip;
};