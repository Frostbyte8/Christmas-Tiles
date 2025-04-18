#include "win32\main_window_view.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    MainWindowView mainWindowView;
    
    if(!mainWindowView.registerSelf(hInstance)) {
        return 0;
    }

    if(!mainWindowView.createWindow(hInstance)) {
        return 0;
    }
    
    return mainWindowView.doLoop();
}
