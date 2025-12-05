#if defined(_MSC_VER) && defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif // defined(_MSC_VER) && defined(_DEBUG)

#include "language_table.h"


#include "view/win32/main_window.h"
#include "view/win32/about_window.h"
#include "view/win32/enter_score_window.h"
#include "view/win32/highscores_window.h"
#include "view/win32/custom_size_window.h"
#include "interface/main_window_interface.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    
#if defined(_MSC_VER) && defined(_DEBUG)
    _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG); 
#endif // defined(_MSC_VER) && defined(_DEBUG)

    if(!languageTable.loadStrings()) {
        MessageBox(NULL, L"Unable to load lang_en.json. If this error persists, reinstalling Christmas Tiles may fix this problem. The Application will now close", L"File Not Found", MB_OK);
        return 0;
    }

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

    if(!AboutWindow::registerSelf(hInstance)) {
        return 0;
    }

    if(!EnterScoreWindow::registerSelf(hInstance)) {
        return 0;
    }

    if(!HighscoresWindow::registerSelf(hInstance)) {
        return 0;
    }

    if(!CustomSizeWindow::registerSelf(hInstance)) {
        return 0;
    }

    // Create Window and do the message loop
    UINT_PTR retVal = 0;
    if(mainWindowView.createWindow()) {
        retVal = mainWindowView.doLoop();
    }


#if defined(_MSC_VER) && defined(_DEBUG)
    
    // In a release mode we don't need to release a bunch of this as the OS
    // cleans it up for us. For debug however, we'll make sure there are no
    // legitimate leaks.
    languageTable.freeStrings();

    return static_cast<unsigned int>(retVal);
#else
    return static_cast<unsigned int>(retVal);
#endif // defined(_MSC_VER) && defined(_DEBUG)
}
