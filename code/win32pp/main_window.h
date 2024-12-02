#ifndef __MAIN_WINDOW_H__
#define __MAIN_WINDOW_H__

#include "..\thirdparty\win32pp\wxx_wincore.h"
#include "..\thirdparty\win32pp\wxx_frame.h"

#include "main_window_views.h"

class MainWindow : public CFrame {

    public:

        MainWindow() {}
        virtual ~MainWindow() {}

        virtual HWND Create(HWND parent = 0) {
            SetView(gameView);
            // Load Settings Here
            return CFrame::Create(parent);
        }

    protected:

        virtual void PreCreate(CREATESTRUCT& cs) {
            CFrame::PreCreate(cs);
        }

    private:

        GameBoardView gameView;
        
        //MainWindow(const MainWindow&);
        //MainWindow& operator= (const MainWindow&);

};

#endif // __MAIN_WINDOW_H__
