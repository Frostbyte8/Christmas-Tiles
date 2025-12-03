#pragma once

namespace MainWindowInterfaceResponses {
    static const int YES    = 6;
    static const int NO     = 7;
    static const int CANCEL = 2;
}

__interface MainWindowInterface {
    public:
        virtual void implDisplayMessage(const wchar_t* message, const wchar_t* title) = 0;
        virtual int implAskYesNoQuestion(const wchar_t* message, const wchar_t* title) = 0;
        virtual void implGameStateChanged(const int& gameState) = 0;
        virtual bool implTryLoadTileset(const wchar_t* fullPath) = 0;

        //virtual bool implRequestCustomSize(unsigned int& newWidth, unsigned int& newHeight) = 0;
        //virtual void implHighScoreAchieved(const wchar_t* name) = 0;
};