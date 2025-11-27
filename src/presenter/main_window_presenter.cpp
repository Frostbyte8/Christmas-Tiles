#include "main_window_presenter.h"

#include <vector>
#include <assert.h>

#ifdef _WIN32

#include <windows.h>

// TODO: __forceline instead of Macro?

// GET_MILLI_COUNT - Must return the number of seconds elapsed since the system
// was started. On Win32 this just calls GetTickCount

#define GET_MILLI_COUNT GetTickCount

// WRITE_INI_STRING - Writes a string to the section given to the INI file.

#define WRITE_INI_STRING(SECTION, KEY_NAME, DATA, PATH_TO_FILE) WritePrivateProfileStringW(SECTION, KEY_NAME, DATA, PATH_TO_FILE)

// READ_INI_UINT - Reads an unsigned 32-bit integer.

#define READ_INI_UINT(SECTION, KEY_NAME, PATH_TO_FILE) GetPrivateProfileIntW(SECTION, KEY_NAME, 0, PATH_TO_FILE)

// READ_INI_STRINGW - Reads a string to the buffer provided

#define READ_INI_STRINGW(SECTION, KEY_NAME, OUT_STRBUF_PTR, BUF_SIZE, PATH_TO_FILE) GetPrivateProfileStringW(SECTION, KEY_NAME, L"", OUT_STRBUF_PTR, BUF_SIZE, PATH_TO_FILE)

#endif // _WIN32

#include "../language_table.h"
#include "../constants.h"
#include "../frost_util.h"

//==============================================================================
// Constructors
//==============================================================================

MainWindowPresenter::MainWindowPresenter() : gameBoard(5, 9, 16) {
    reset(); // Just to reduce code reuse.
}

__forceinline void MainWindowPresenter::reset() {
    selectedIndex1      = 0;
    selectedIndex2      = 0;
    points              = 50;
    score               = 0;
    matchesMade         = 0;
    milliStartTime      = 0;
    milliElapsedTime    = 0;
    milliPointDelta     = 0;
    gameState           = GameState::STATE_GAMEWON;
}

//==============================================================================
// Public Functions
//==============================================================================
    
//------------------------------------------------------------------------------
// onClickTile - Call when the tile is clicked
// Note that yIndex is over written.
//------------------------------------------------------------------------------

int MainWindowPresenter::tryFlipTileAtCoodinates(uint8_t& xIndex, uint8_t& yIndex) {

    if(gameState != GameState::STATE_PLAYING) {
        
        if(gameState == GameState::STATE_GAMEWON) {
            return GameBoardFlipErrors::GameAlreadyWon;
        }
        else if(!tryUnpause()) {
            return GameBoardFlipErrors::CannotUnpause;
        }
    }

    // Unflip tiles if necessary
    unflipTiles();

    const uint8_t MAX_X = gameBoard.getWidth() - 1;
    const uint8_t MAX_Y = gameBoard.getHeight() - 1;

    // Validate indices

    if(xIndex > MAX_X || yIndex > MAX_Y) {
        return GameBoardFlipErrors::OutOfBounds;
    }

    // Next, make sure the tile has not been flipped, and if it has not,
    // flip it
    yIndex = (yIndex * (MAX_X+1)) + xIndex;
    const std::vector<TileData>& tiles = gameBoard.getTiles();

    if(tiles[yIndex].flags) {
        return GameBoardFlipErrors::AlreadyFlipped;
    }
    
    gameBoard.setTileFlipped(yIndex);
    
    if(selectedIndex1 == GameBoardConstants::NO_SELECTED_INDEX) {
        selectedIndex1 = yIndex; // Track that we flipped a tile so far.
    }
    else {

        // This is our second selected tile, check if the match
        if(tiles[selectedIndex1].ID == tiles[yIndex].ID) {
            
            gameBoard.setTilesMatched(selectedIndex1, yIndex);
            selectedIndex1 = GameBoardConstants::NO_SELECTED_INDEX;
            
            matchesMade++;

            score += points;
            if(score > ChristmasTilesConstants::MAX_SCORE) {
                score = ChristmasTilesConstants::MAX_SCORE;
            }
            points = 50;

            if(matchesMade == gameBoard.getMatchesNeeded()) {
                updateElapsedTime();
                gameState = GameState::STATE_GAMEWON;
                mainWindow->implGameStateChanged(gameState);
            }

            return GameBoardFlipErrors::TilesMatched;

        }

        // Tiles do not match, but keep track of this until the next selection

        points = points > 5 ? points - 5 : 0;

        selectedIndex2 = yIndex;
        return GameBoardFlipErrors::TilesNotMatched;

    }

    return GameBoardFlipErrors::TileFlipped;

}

//------------------------------------------------------------------------------
// tryAddScore - Attempt to add an entry to the score table
//------------------------------------------------------------------------------

bool MainWindowPresenter::tryAddScore(wchar_t* name, const size_t& index) {

    time_t currentTime;
    tm timeInfo;

    time(&currentTime);
    localtime_s(&timeInfo, &currentTime);
    
    
    scoreTable.tryAddScore(name, score, static_cast<uint16_t>(1900 + timeInfo.tm_year), 
                                        static_cast<uint8_t>(timeInfo.tm_mon + 1), 
                                        static_cast<uint8_t>(timeInfo.tm_mday));
    return true; 
}

//------------------------------------------------------------------------------
// tryTogglePause - Attempt to Pause/Unpause the game
//------------------------------------------------------------------------------

bool MainWindowPresenter::tryTogglePause() {

    if(gameState == GameState::STATE_PLAYING) {
        return tryPause();
    }
    else if(gameState == GameState::STATE_PAUSED) {
        return tryUnpause();
    }

    return false;

}

//------------------------------------------------------------------------------
// getElapsedTime - Get how much time has elapsed. This function will also
// attempt to update how much time has elapsed.
//------------------------------------------------------------------------------

const uint32_t& MainWindowPresenter::getElapsedTime() {

    if(gameState == GameState::STATE_PLAYING) {
        updateElapsedTime();
    }

    return milliElapsedTime;
}

//------------------------------------------------------------------------------
// tryUpdateGameBoard - Used when changing size, tile types, or both.
// Note: Method only checks IGNORE_WIDTH when ignoring sizes. If Width is valid,
// it assumes height is too.
//------------------------------------------------------------------------------

bool MainWindowPresenter::tryUpdateGameBoard(uint8_t newWidth, uint8_t newHeight, uint8_t tileTypes) {
    
    if(gameState != GameState::STATE_GAMEWON) {
        const int retVal = mainWindow->implAskYesNoQuestion(GET_LANG_STR(LangID::ACTION_STARTS_NEW_GAME_TEXT), GET_LANG_STR(LangID::ACTION_STARTS_NEW_GAME_TITLE));

        if(retVal != MainWindowInterfaceResponses::YES) {
            return false;
        }
    }

    if(newWidth == WindowPresenterConstants::IGNORE_WIDTH) {
        newWidth = gameBoard.getWidth();
        newHeight = gameBoard.getHeight();
    }

    if(tileTypes == WindowPresenterConstants::IGNORE_NUMTILES) {
        tileTypes = gameBoard.getNumTileTypes();
    }

    FrostUtil::ClampInts(newWidth, GameBoardConstants::MIN_WIDTH, GameBoardConstants::MAX_WIDTH);
    FrostUtil::ClampInts(newHeight, GameBoardConstants::MIN_HEIGHT, GameBoardConstants::MAX_HEIGHT);

    gameBoard = GameBoard(newWidth, newHeight, tileTypes);

    tryNewGame();
    return true;

}

//------------------------------------------------------------------------------
// RequestNewGame - The user has requested a new game
//------------------------------------------------------------------------------

bool MainWindowPresenter::requestNewGame() {
    
    if(gameState != GameState::STATE_GAMEWON) {

        const int retVal = mainWindow->implAskYesNoQuestion(GET_LANG_STR(LangID::GAME_IN_PROGRESS_NEWGAME_TEXT), GET_LANG_STR(LangID::GAME_IN_PROGRESS_NEWGAME_TITLE));

        if(retVal != MainWindowInterfaceResponses::YES) {
            return false;
        }

    }

    return tryNewGame();
}

//------------------------------------------------------------------------------
// unflipTiles - Unflips tiles if necessary
//------------------------------------------------------------------------------

inline void MainWindowPresenter::unflipTiles() {

    if(selectedIndex2 != GameBoardConstants::NO_SELECTED_INDEX) {
        
        assert(selectedIndex1 != GameBoardConstants::NO_SELECTED_INDEX);

        gameBoard.setTilesUnflipped(selectedIndex1, selectedIndex2);
        selectedIndex1 = GameBoardConstants::NO_SELECTED_INDEX;
        selectedIndex2 = GameBoardConstants::NO_SELECTED_INDEX;

    }

}

//------------------------------------------------------------------------------
// writeSettings - Writes to an ini file.
//------------------------------------------------------------------------------

bool MainWindowPresenter::writeSettings() {
    wchar_t buffer[4] = {0};
    
    wsprintf(buffer, L"%d", gameBoard.getWidth());
    if( !WRITE_INI_STRING(L"settings", L"width", buffer, L".\\ChristmasTiles.ini") ) {
        return false;
    }

    wsprintf(buffer, L"%d", gameBoard.getHeight());
    if(!WRITE_INI_STRING(L"settings", L"height", buffer, L".\\ChristmasTiles.ini")) {
        return false;
    }
    // TODO: Tileset
    return true;

}

//------------------------------------------------------------------------------
// readScores - Writes scores to an INI file
//------------------------------------------------------------------------------

bool MainWindowPresenter::readScores() {

    wchar_t buffer[ScoreTableConstants::MAX_NAME_LENGTH+1] = {0};
    wchar_t keyName[16] = {0};

    //GetPrivateProfileString(L"settings", L"tileset", NULL, filePath, MAX_PATH-1, L".\\ChristmasTiles.ini");
    
    for(size_t i = 0; i < ScoreTableConstants::MAX_SCORES; ++i) {
    
        ScoreT score;
        buffer[ScoreTableConstants::MAX_NAME_LENGTH] = 0;

        bool defaultScore = false;

        wsprintf(keyName, L"date%d", i);
        READ_INI_STRINGW(L"scores", keyName, buffer, ScoreTableConstants::MAX_NAME_LENGTH, L".\\scores.ini");

        if(wcslen(buffer) < 10) {

            defaultScore = true;


        }
        else {

            // TODO: This could be much better.

            tm dayOfScore = {0};


            wchar_t* token = wcstok(buffer, L"/");
            if(!token) {
                defaultScore = true;
            }
            
            dayOfScore.tm_year =  _wtoi(token) - 1900;
            score.year = _wtoi(token);

            token = wcstok(NULL, L"/");
            if(!token) {
                defaultScore = true;
            }

            dayOfScore.tm_mon =  _wtoi(token) - 1;
            score.month = _wtoi(token);

            token = wcstok(NULL, L"/");
            if(!token) {
                defaultScore = true;
            }
            dayOfScore.tm_mday =  _wtoi(token);
            score.day = _wtoi(token);

            if(mktime(&dayOfScore) == -1) {
                defaultScore = true;
            }

        }

        if(defaultScore) {
            score.year = 1995;
            score.month = 12;
            score.day = 25;
            score.name = NULL;
            score.score = 0;
        }
        else {

            wsprintf(keyName, L"score%d", i);
            score.score = READ_INI_UINT(L"scores", keyName, L".\\scores.ini");

            wsprintf(keyName, L"name%d", i);
            READ_INI_STRINGW(L"scores", keyName, buffer, ScoreTableConstants::MAX_NAME_LENGTH, L".\\scores.ini");
            score.name = buffer;
        }

        // TODO: pass in a score object
        scoreTable.tryAddScore(score.name, score.score, score.year, score.month, score.day);

    }

    return true;
}

//------------------------------------------------------------------------------
// writeScores - Writes scores to an INI file
//------------------------------------------------------------------------------

bool MainWindowPresenter::writeScores() {

    wchar_t buffer[ScoreTableConstants::MAX_NAME_LENGTH+1] = {0};
    wchar_t keyName[16] = {0};
    const std::vector<ScoreT>& scores = scoreTable.getScores();

    // TODO: Clear old scores ini first before writing a new one.
    
    for(size_t i = 0; i < scores.size(); ++i) {

        swprintf_s(buffer, ScoreTableConstants::MAX_NAME_LENGTH, L"%s", scores[i].name);
        wsprintf(keyName, L"name%d", i);
        if(!WRITE_INI_STRING(L"scores", keyName, buffer, L".\\scores.ini")) {
            return false;
        }
        
        wsprintf(buffer, L"%d", scores[i].score);
        wsprintf(keyName, L"score%d", i);
        if(!WRITE_INI_STRING(L"scores", keyName, buffer, L".\\scores.ini")) {
            return false;
        }

        wsprintf(buffer, L"%d/%0d/%0d", scores[i].year, scores[i].month, scores[i].day);
        wsprintf(keyName, L"date%d", i);
        if(!WRITE_INI_STRING(L"scores", keyName, buffer, L".\\scores.ini")) {
            return false;
        }

    }

    return true;

}

//==============================================================================
// Private Functions
//==============================================================================


//------------------------------------------------------------------------------
// tryNewGame - Attempts to start a new game
//------------------------------------------------------------------------------

bool MainWindowPresenter::tryNewGame() {

    reset();
    gameBoard.tryNewGame();

    gameState = GameState::STATE_NOT_STARTED;
    mainWindow->implGameStateChanged(gameState);
    
    return true;

}

//------------------------------------------------------------------------------
// tryPause - Attempt to pause the game. If it does, it will also update how
// much time the game has been played elapsed before the game was paused.
//------------------------------------------------------------------------------

bool MainWindowPresenter::tryPause() {

    if(gameState != GameState::STATE_PLAYING) {
        // You can only pause games that are being played.
        return false;
    }

    updateElapsedTime();
    gameState = GameState::STATE_PAUSED;
    mainWindow->implGameStateChanged(gameState);

    return true;

}

//------------------------------------------------------------------------------
// tryUnpause - Attempts to un pause the game and resume the how much time the
// game has been played.
//------------------------------------------------------------------------------

bool MainWindowPresenter::tryUnpause() {

    if(gameState > GameState::STATE_PAUSED) {
        // Cannot Unpause if game is playing or won.
        return false;
    }
   
    milliStartTime = GET_MILLI_COUNT();
    gameState = GameState::STATE_PLAYING;
    mainWindow->implGameStateChanged(gameState);

    return true;
}

//------------------------------------------------------------------------------
// updateElapsedTime - Updates how long the game has been played. You must
// ensure that the game is not paused before calling this function.
//------------------------------------------------------------------------------

void MainWindowPresenter::updateElapsedTime() {

    assert(gameState == GameState::STATE_PLAYING);

    const uint32_t timeNow = GET_MILLI_COUNT();
    milliElapsedTime += (timeNow - milliStartTime);
    milliStartTime = timeNow;

    if(milliElapsedTime > milliPointDelta + 999) {
        
        uint32_t seconds = (milliElapsedTime - milliPointDelta) / 1000;
        
        if(points < seconds) {
            points = 0;
        }
        else {
            points -= static_cast<uint8_t>(seconds);
        }

        milliPointDelta += (seconds * 1000);
    }

}