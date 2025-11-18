#if defined(_MSC_VER) && defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif // defined(_MSC_VER) && defined(_DEBUG)

#include "language_table.h"


#include "view/win32/main_window.h"
#include "interface/main_window_interface.h"


int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    
#if defined(_MSC_VER) && defined(_DEBUG)
    _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG); 
#endif // defined(_MSC_VER) && defined(_DEBUG)

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


#if defined(_MSC_VER) && defined(_DEBUG)
    
    UINT retVal = mainWindowView.doLoop();

    // In a release mode we don't need to release a bunch of this as the OS
    // cleans it up for us. For debug however, we'll make sure there are no
    // legitimate leaks.
    languageTable.freeStrings();

    return retVal;
#else
    return mainWindowView.doLoop();
#endif // defined(_MSC_VER) && defined(_DEBUG)
}
