#include "view/win32/main_window.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
        
    MainWindowView mainWindowView(hInstance);
    
    if(!mainWindowView.registerSelf()) {
        return 0;
    }

    // Register additional controls

    if(!GamePanel::registerSelf(hInstance)) {
        return 0;
    }

    if(!DynamicLabel::registerSelf(hInstance)) {
        return 0;
    }

    // Create Window and do the message loop

    if(!mainWindowView.createWindow()) {
        return 0;
    }
    
    return mainWindowView.doLoop();
}
