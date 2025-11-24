#pragma once

#include <vector>

#define GET_LANG_STR(x) languageTable.getStrings()[x]

namespace LangID {
    enum LangID {
        ERROR_STR_NOT_FOUND = -1,
        APP_TITLE = 0,
        OK_BUTTON_CAPTION,
        CANCEL_BUTTON_CAPTION,
        SCORE_BOX_CAPTION,
        POINTS_BOX_CAPTION,
        TIME_BOX_CAPTION,
        PAUSE_BUTTON_CAPTION,
        UNPAUSE_BUTTON_CAPTION,
        MENU_NEW_GAME,
        MENU_PAUSE,
        MENU_UNPAUSE,
        MENU_HIGHSCORES,
        MENU_EXIT,
        MENU_CHANGE_TILESET,
        MENU_CHANGE_LANGUAGE,
        MENU_CUSTOM_SIZE,
        MENU_HELP_ITEM,
        MENU_ABOUT,
        MENU_FILE,
        MENU_OPTIONS,
        MENU_HELP,
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
        ACTION_STARTS_NEW_GAME_TEXT,
        ACTION_STARTS_NEW_GAME_TITLE,
        ENTER_SCORE_WINDOW_TITLE,
        ENTER_SCORE_MESSAGE_1,
        ENTER_SCORE_MESSAGE_2,
        HIGHSCORES_TITLE,
        HIGHSCORES_TEXT,
        ABOUT_TITLE,
        VERSION_TEXT,
        COPYRIGHT_TEXT,
        CUSTOM_SIZE_TITLE,
        CUSTOM_SIZE_TEXT,
        LANG_ID_COUNT
    };
}

#define PROGRAM_TITLE L"Christmas Tiles\0"

class LanguageTable {

    public:
    LanguageTable() {

        /*
        languageStrings.resize(1);

        wchar_t* str = NULL;
        const size_t stringLength = wcslen(PROGRAM_TITLE);
        str = (wchar_t*)malloc(sizeof(wchar_t) * (stringLength+1));       

        wcsncpy_s(str, stringLength+1, PROGRAM_TITLE, stringLength);
        str[stringLength] = 0;
        */

        //languageStrings[LangID::APP_TITLE] = str;
    }

    void loadStrings();

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

extern LanguageTable languageTable;

#undef PROGRAM_TITLE