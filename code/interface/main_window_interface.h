#pragma once
#ifndef __MAIN_WINDOW_INTERFACE_H__
#define __MAIN_WINDOW_INTERFACE_H__

#include "..\stdint_compat.h"

class MainWindowInterface {
    public:
        //virtual void displayMessageBox(const uint32_t& langID) = 0;
        virtual void gameWon() = 0;
};

#endif // __MAIN_WINDOW_INTERFACE_H__
