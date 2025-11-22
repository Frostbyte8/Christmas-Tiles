#pragma once

#include <windows.h>
#include "subclassed_controls.h"
#include "../../model/gameboard.h"
#include "../../presenter/main_window_presenter.h"

// TODO: MainWindowInterface

class GamePanel : public WndAsClass<GamePanel> {
    friend WndAsClass;

    public:
        GamePanel();
        static bool registerSelf(HINSTANCE hInst);
        bool createWindow(const HINSTANCE& hInst, const HWND& parent, const HMENU& ID);
        UINT doLoop();

        inline const HWND& getHandle() const { return hWnd; }
        void setWindowPresenter(MainWindowPresenter* mwp) {
            windowPresenter = mwp;
        }

        inline const LONG& getTilesetWidth() const { return bmpWidth; }
        inline const LONG& getTilesetHeight() const { return bmpHeight; }

        void updateVirtualSize(const WORD& numXTiles, const WORD& numYTiles);

    private:

        void onHScroll(const WORD& dir);
        void onVScroll(const WORD& dir);
        void onSize(const WORD& newWidth, const WORD& newHeight);
        void onClick(WORD xPos, WORD yPos);
        void onPaint();
        LRESULT windowProc(const UINT& msg, const WPARAM wParam, const LPARAM lParam);

        MainWindowPresenter const *windowPresenter;

        HWND hWnd;
        static bool isRegistered;
        
        HBITMAP tilesetBMP;
        LONG bmpWidth;
        LONG bmpHeight;
        int xOffset;
        int yOffset;
        WORD virtualWidth;
        WORD virtualHeight;
};