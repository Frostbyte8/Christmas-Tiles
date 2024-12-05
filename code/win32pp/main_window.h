#pragma once
#ifndef __MAIN_WINDOW_H__
#define __MAIN_WINDOW_H__

#include "..\thirdparty\win32pp\wxx_wincore.h"
#include "..\thirdparty\win32pp\wxx_frame.h"
#include "..\win32\window_metrics.h"
#include "main_window_views.h"

class MainWindow : public CFrame {

    public:

        MainWindow() : prevMonitor(0) {}
        virtual ~MainWindow() {}

        virtual HWND Create(HWND parent = 0);

        virtual int OnCreate(CREATESTRUCT& cs);



    protected:

        virtual void PreCreate(CREATESTRUCT& cs);
        virtual LRESULT WndProc(UINT msg, WPARAM wParam, LPARAM lPARAM);

    private:

        void onWindowMoved();

        WindowMetrics wMetrics;
        HMONITOR prevMonitor;
        MainWindowView mainView;
        
        MainWindow(const MainWindow&);
        MainWindow& operator= (const MainWindow&);

};

#endif // __MAIN_WINDOW_H__
