#pragma once
#ifndef __MAIN_WINDOW_H__
#define __MAIN_WINDOW_H__

#include "..\thirdparty\win32pp\wxx_wincore.h"
#include "..\thirdparty\win32pp\wxx_frame.h"
#include "..\interface\main_window_interface.h"
#include "..\presenter\game_presenter.h"
#include "main_window_views.h"

#include "..\win32\window_metrics.h"
#include <windows.h>
#define NOMINMAX
// [TODO]: Something keeps Undefing Min and Max, but who? Look into this more
// but for now, use the versions found in min and max
namespace Gdiplus
{
  using std::min;
  using std::max;
};
#include <gdiplus.h>
#undef NOMINMAX

/*
class GameBoardPanel : public CWnd {

};
*/

class MainWindow : public CFrame, public MainWindowInterface {

    public:

        MainWindow();

        virtual ~MainWindow() {}
        virtual HWND Create(HWND parent = 0);
        virtual int OnCreate(CREATESTRUCT& cs);

        // Interface Implementation
        virtual void displayMessageBox(const uint32_t& langID);

    protected:

        virtual void PreCreate(CREATESTRUCT& cs);
        virtual LRESULT WndProc(UINT msg, WPARAM wParam, LPARAM lPARAM);

    private:

        void onWindowMoved();
        void loadResources();

        GamePresenter gamePresenter;

        Gdiplus::Bitmap* tileset;
        WindowMetrics wMetrics;
        HMONITOR prevMonitor;
        MainWindowView mainView;
        
        MainWindow(const MainWindow&);
        MainWindow& operator= (const MainWindow&);

};

#endif // __MAIN_WINDOW_H__
