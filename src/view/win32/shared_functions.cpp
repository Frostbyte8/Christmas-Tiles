#include "shared_functions.h"

bool CALLBACK ChangeControlsFont(HWND child, LPARAM font) {
    SendMessage(child, WM_SETFONT, font, TRUE);
    return true;
}