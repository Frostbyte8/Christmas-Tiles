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
        
        bool createWindow(const HINSTANCE& hInst, const HWND& parent, const HMENU& ID);
        static bool registerSelf(HINSTANCE hInst);

        // Accessors

        inline const HWND& getHandle() const { return hWnd; }
        LONG getNumTileTypes() const { return bmpWidth / bmpHeight; }
        const LONG& getTileSize() const { return bmpHeight; }

        // Mutators

        void setWindowPresenter(MainWindowPresenter* mwp) {
            windowPresenter = mwp;
        }

        // Public Functions

        bool changeTileset();
        void updateVirtualSize(const WORD& numXTiles, const WORD& numYTiles);

    private:

        void onHScroll(const WORD& dir, const WORD& pos);
        void onVScroll(const WORD& dir, const WORD& pos);
        void onPaint();
        void onSize();

        LRESULT windowProc(const UINT& msg, const WPARAM wParam, const LPARAM lParam);

        MainWindowPresenter const *windowPresenter;

        HWND        hWnd;
        static bool isRegistered;
        
        HBITMAP tilesetBMP;

        LONG    bmpWidth;
        LONG    bmpHeight;
        int     xOffset;
        int     yOffset;
        WORD    virtualWidth;
        WORD    virtualHeight;
        
};