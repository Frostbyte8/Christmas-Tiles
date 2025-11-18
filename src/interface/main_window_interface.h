#pragma once

class MainWindowInterface {
    public:
        virtual void implDisplayTestMessage() = 0;
        virtual void implGameStateChanged(const int& gameState) = 0;
        virtual int implAskYesNoQuestion(const wchar_t* message, const wchar_t* title) = 0;
};