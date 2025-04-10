#ifndef __GAMEBOARD_PANEL_H__
#define __GAMEBOARD_PANEL_H__

#include <windows.h>
#include "subclassed_controls.h"
#include "../presenter/game_presenter.h"

#define BP_CLICKED  WM_APP+1

class GameBoardPanel : public WndAsClass <GameBoardPanel> {
    friend WndAsClass;
    public:

        static wchar_t* getWndClassName();
        void setTileset(HBITMAP newTileset);

        bool registerSelf(HINSTANCE hInstance);
        LRESULT windowProc(const UINT& msg, const WPARAM wParam, const LPARAM lParam);


        GamePresenter* gamePresenter;

    private:

        HWND    window;
        HWND    parent;
        HBITMAP tileset;
        HBRUSH  gameBG;

        LRESULT onCreate(const CREATESTRUCT* const cs);
        LRESULT onPaint();
};

#endif // __GAMEBOARD_PANEL_H__
