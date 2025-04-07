#pragma once
#ifndef __MAIN_WINDOW_INTERFACE_H__
#define __MAIN_WINDOW_INTERFACE_H__

#include <string>
#include "..\stdint_compat.h"

namespace MessageBoxReponses {
    enum MessageBoxReponses {
        OK      = 1,
        YES     = 6,
        NO      = 7,
        CANCEL  = 2,
    };
}

namespace MessageBoxTypes {
    enum MessageBoxTypes {
        OK              = 0,
        YESNO           = 4,
        YESNOCANCEL     = 3,
    };
}

class MainWindowInterface {
    public:
        //virtual void displayMessageBox(const uint32_t& langID) = 0;
        virtual int askQuestion(const std::string& message, const std::string& title, const int& mbType) = 0;
        virtual void gameWon() = 0;
};

#endif // __MAIN_WINDOW_INTERFACE_H__
