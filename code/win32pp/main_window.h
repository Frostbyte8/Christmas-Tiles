#pragma once
#ifndef __MAIN_WINDOW_H__
#define __MAIN_WINDOW_H__

#include "..\thirdparty\win32pp\wxx_wincore.h"
#include "..\thirdparty\win32pp\wxx_frame.h"
#include "..\interface\main_window_interface.h"
#include "..\presenter\game_presenter.h"
#include "main_window_views.h"

#include "..\win32\window_metrics.h"

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

        GamePresenter gamePresenter;

        WindowMetrics wMetrics;
        HMONITOR prevMonitor;
        MainWindowView mainView;
        
        MainWindow(const MainWindow&);
        MainWindow& operator= (const MainWindow&);

};

#endif // __MAIN_WINDOW_H__
