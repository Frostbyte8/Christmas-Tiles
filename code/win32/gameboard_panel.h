#ifndef __GAMEBOARD_PANEL_H__
#define __GAMEBOARD_PANEL_H__

#include <windows.h>
#include "subclassed_controls.h"
#include "../presenter/game_presenter.h"

class GameBoardPanel : public WndAsClass <GameBoardPanel> {

    public:
        bool registerSelf(HINSTANCE hInstance);
        LRESULT windowProc(const UINT& msg, const WPARAM wParam, const LPARAM lParam);
        HWND window;
        HBITMAP tileset;
        LRESULT onPaint();
        static wchar_t* getWndClassName() { return L"GameBoardClass"; }
        GamePresenter* gamePresenter;

};

#endif // __GAMEBOARD_PANEL_H__
