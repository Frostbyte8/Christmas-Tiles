#pragma once

#include <vector>

namespace LangID {
    enum LangID {
        APP_TITLE               = 0,
        SCORE_BOX_CAPTION,
        POINTS_BOX_CAPTION,
        TIME_BOX_CAPTION,
        PAUSE_BUTTON_CAPTION,
        UNPAUSE_BUTTON_CAPTION,
        YOU_WIN_MESSAGE_TEXT,
        YOU_WIN_MESSAGE_TITLE,
        GIVE_UP_MESSAGE_TEXT,
        GIVE_UP_MESSAGE_TITLE,
        GAME_IN_PROGRESS_QUIT_TEXT,
        GAME_IN_PROGRESS_QUIT_TITLE,
        GAME_IN_PROGRESS_NEWGAME_TEXT,
        GAME_IN_PROGRESS_NEWGAME_TITLE,
        DO_YOU_WANT_PLAYAGAIN_TEXT,
        DO_YOU_WANT_PLAYAGAIN_TITLE,
        LANG_ID_COUNT
    };
}

#define PROGRAM_TITLE L"Christmas Tiles\0"

class LanguageTable {

    public:
    LanguageTable() {
        languageStrings.resize(1);

        wchar_t* str = NULL;
        const size_t stringLength = wcslen(PROGRAM_TITLE);
        str = (wchar_t*)malloc(sizeof(wchar_t) * (stringLength+1));       

        str[stringLength] = 0;
        wcsncpy(str, PROGRAM_TITLE, stringLength);


        languageStrings[LangID::APP_TITLE] = str;
    }

    void freeStrings() {
        for(size_t i = 0; i < languageStrings.size(); ++i) {
            free(languageStrings[i]);
            languageStrings[i] = NULL;
        }

        languageStrings.resize(0);
    }

    const inline std::vector<wchar_t*>& getStrings() const { return languageStrings; }

    private:
        std::vector<wchar_t*> languageStrings;

};

#undef PROGRAM_TITLE