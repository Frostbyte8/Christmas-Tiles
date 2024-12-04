#ifndef __MAIN_WINDOW_VIEWS_H__
#define __MAIN_WINDOW_VIEWS_H__

#include "..\thirdparty\win32pp\wxx_wincore.h"
#include "..\thirdparty\win32pp\wxx_stdcontrols.h"

class MainWindowView : public CWnd {

    public:

        MainWindowView() {}
        LRESULT WndProc(UINT msg, WPARAM wParam, LPARAM lParam) { return WndProcDefault(msg, wParam, lParam); }

    protected:
        
        virtual int OnCreate(CREATESTRUCT& cs);

        virtual void PreCreate(CREATESTRUCT& cs) {
            //cs.dwExStyle = WS_EX_CLIENTEDGE;
        }

        virtual void PreRegisterClass(WNDCLASS& wc) {
            wc.hCursor          = ::LoadCursor(NULL, IDC_ARROW);
            wc.hbrBackground    = (HBRUSH)(COLOR_BTNFACE + 1);
            wc.lpszClassName    = "MainWindowView";
        }

    private:
        CButton     grpInfo[3];
        CButton     btnPause;

};

class GameBoardView : public CWnd {

    public:
        GameBoardView() {}
        LRESULT WndProc(UINT msg, WPARAM wParam, LPARAM lParam) { return WndProcDefault(msg, wParam, lParam); }
};

#endif // __MAIN_WINDOW_VIEWS_H__
