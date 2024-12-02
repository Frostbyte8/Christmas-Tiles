#ifndef __MAIN_WINDOW_VIEWS_H__
#define __MAIN_WINDOW_VIEWS_H__

#include "..\thirdparty\win32pp\wxx_wincore.h"

class GameBoardView : public CWnd {

    public:
        GameBoardView() {}
        LRESULT WndProc(UINT msg, WPARAM wParam, LPARAM lParam) { return WndProcDefault(msg, wParam, lParam); }
};

#endif // __MAIN_WINDOW_VIEWS_H__
