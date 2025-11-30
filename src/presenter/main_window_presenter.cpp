#include "main_window_presenter.h"

#include <vector>
#include <assert.h>

#ifdef _WIN32

#include <windows.h>

// GET_MILLI_COUNT - Must return the number of seconds elapsed since the system
// was started. On Win32 this just calls GetTickCount

#define GET_MILLI_COUNT GetTickCount

// WRITE_INI_STRING - Writes a string to the section given to the INI file.

#define WRITE_INI_STRING(SECTION, KEY_NAME, DATA, PATH_TO_FILE) WritePrivateProfileStringW(SECTION, KEY_NAME, DATA, PATH_TO_FILE)

// READ_INI_UINT - Reads an unsigned 32-bit integer.

#define READ_INI_UINT(SECTION, KEY_NAME, PATH_TO_FILE) GetPrivateProfileIntW(SECTION, KEY_NAME, 0, PATH_TO_FILE)

// READ_INI_STRINGW - Reads a string to the buffer provided

#define READ_INI_STRINGW(SECTION, KEY_NAME, OUT_STRBUF_PTR, BUF_SIZE, PATH_TO_FILE) GetPrivateProfileStringW(SECTION, KEY_NAME, L"", OUT_STRBUF_PTR, BUF_SIZE, PATH_TO_FILE)

// DoesFileExist - Checks if a File Exists

__forceinline BOOL DoesFileExist(const wchar_t* path) {
    const DWORD fileAttrib = GetFileAttributes(path);
    return (fileAttrib != INVALID_FILE_ATTRIBUTES && !(fileAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

static const unsigned int FILE_PATH_MAX = MAX_PATH;

#endif // _WIN32

#include "../language_table.h"
#include "../constants.h"
#include "../frost_util.h"

//==============================================================================
// Constructors
//==============================================================================

MainWindowPresenter::MainWindowPresenter(MainWindowInterface& inMWI) : mainWindow(inMWI) {
    reset();
    windowData.pathToTileset = NULL;
}

__forceinline void MainWindowPresenter::reset() {

    selectedIndex1      = 0;
    selectedIndex2      = 0;
    matchesMade         = 0;
    milliStartTime      = 0;
    milliElapsedTime    = 0;
    milliPointDelta     = 0;

    windowData.gameState        = GameState::STATE_NO_GAME;
    windowData.points           = 50;
    windowData.score            = 0;
    
}

//==============================================================================
// Accessors
//==============================================================================

//------------------------------------------------------------------------------
// getElapsedTime - Get how much time has elapsed. This function will also
// attempt to update how much time has elapsed.
//------------------------------------------------------------------------------

const uint32_t& MainWindowPresenter::getElapsedTime() {

    if(windowData.gameState == GameState::STATE_PLAYING) {
        updateElapsedTime();
    }

    return milliElapsedTime;
}

//==============================================================================
// Public Functions
//==============================================================================

//------------------------------------------------------------------------------
// getScorePosition - Checks to see what position the score is in.
//------------------------------------------------------------------------------

const size_t& MainWindowPresenter::getScorePosition() const {
    return getScoreTable().isNewHighscore(windowData.score);
}

//------------------------------------------------------------------------------
// requestNewGame - The user has requested a new game
//------------------------------------------------------------------------------

int MainWindowPresenter::requestNewGame() {
    
    if(!shouldEndGameIfPlaying(LangID::GAME_IN_PROGRESS_NEWGAME_TEXT)) {
        return -1;
    }

    const GameBoardDimensions& boardDimensions = gameBoard.getBoardDimensions();

    if(!createNewGameBoard(boardDimensions.width, boardDimensions.height, gameBoard.getNumTileTypes())) {
        return 0;
    }

    return 1;
}

//------------------------------------------------------------------------------
// tryAddScore - Attempt to add an entry to the score table
//------------------------------------------------------------------------------

bool MainWindowPresenter::tryAddScore(wchar_t* name, const size_t& index) {

    time_t currentTime;
    tm timeInfo;

    time(&currentTime);
    localtime_s(&timeInfo, &currentTime);
    
    
    scoreTable.tryAddScore(name, windowData.score, static_cast<uint16_t>(1900 + timeInfo.tm_year), 
                                                   static_cast<uint8_t>(timeInfo.tm_mon + 1), 
                                                   static_cast<uint8_t>(timeInfo.tm_mday));
    return true; 
}

//------------------------------------------------------------------------------
// tryFlipTileAtCoodinates - Call when the tile is clicked.
// Note that yIndex is over written.
//------------------------------------------------------------------------------

int MainWindowPresenter::tryFlipTileAtCoodinates(unsigned int& xIndex, unsigned int& yIndex) {

    if(windowData.gameState != GameState::STATE_PLAYING) {

        switch(windowData.gameState) {
            
            default:
            
                if(!tryUnpause()) {
                    return GameBoardFlipErrors::CannotUnpause;
                }
                break;

            case GameState::STATE_GAMEWON:
                return GameBoardFlipErrors::GameAlreadyWon;

            case GameState::STATE_NO_GAME:
                return GameBoardFlipErrors::GameNotCreated;

        }
        
    }

    // Unflip tiles if necessary
    unflipTiles();

    const GameBoardDimensions& boardDimensions = gameBoard.getBoardDimensions();

    // Validate indices

    if(xIndex > boardDimensions.width  - 1 || yIndex > boardDimensions.height - 1) {
        return GameBoardFlipErrors::OutOfBounds;
    }

    // Next, make sure the tile has not been flipped, and if it has not,
    // flip it
    yIndex = (yIndex * (boardDimensions.width)) + xIndex;
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

            windowData.score += windowData.points;
            if(windowData.score > ChristmasTilesConstants::MAX_SCORE) {
                windowData.score = ChristmasTilesConstants::MAX_SCORE;
            }
            
            windowData.points = 50;

            if(matchesMade == gameBoard.getMatchesNeeded()) {
                updateElapsedTime();
                windowData.gameState = GameState::STATE_GAMEWON;
                mainWindow.implGameStateChanged(windowData.gameState);
            }

            return GameBoardFlipErrors::TilesMatched;

        }

        // Tiles do not match, but keep track of this until the next selection

        windowData.points = windowData.points > 5 ? windowData.points - 5 : 0;

        selectedIndex2 = yIndex;
        return GameBoardFlipErrors::TilesNotMatched;

    }

    return GameBoardFlipErrors::TileFlipped;

}

//------------------------------------------------------------------------------
// tryTogglePause - Attempt to Pause/Unpause the game
//------------------------------------------------------------------------------

bool MainWindowPresenter::tryTogglePause() {

    if(windowData.gameState == GameState::STATE_PLAYING) {
        return tryPause();
    }
    else if(windowData.gameState == GameState::STATE_PAUSED) {
        return tryUnpause();
    }

    return false;

}

//------------------------------------------------------------------------------
// tryUpdateGameBoard - Used when changing size, tile types, or both.
// Note: Method only checks IGNORE_WIDTH when ignoring sizes. If Width is valid,
// it assumes height is too.
//------------------------------------------------------------------------------

int MainWindowPresenter::tryUpdateGameBoard(unsigned int& newWidth, unsigned int& newHeight, uint8_t tileTypes) {
    
    if(!shouldEndGameIfPlaying(LangID::ACTION_STARTS_NEW_GAME_TEXT)) {
        return -1;
    }

    if(newWidth == WindowPresenterConstants::IGNORE_WIDTH) {

        const GameBoardDimensions& boardDimensions = gameBoard.getBoardDimensions();

        newWidth = boardDimensions.width;
        newHeight = boardDimensions.height;

    }
    else {
        assert(newHeight != WindowPresenterConstants::IGNORE_HEIGHT); // Read the comment at the top.
    }

    if(tileTypes == WindowPresenterConstants::IGNORE_NUMTILES) {
        tileTypes = gameBoard.getNumTileTypes();
    }

    FrostUtil::ClampInts(newWidth, GameBoardConstants::MIN_WIDTH, GameBoardConstants::MAX_WIDTH);
    FrostUtil::ClampInts(newHeight, GameBoardConstants::MIN_HEIGHT, GameBoardConstants::MAX_HEIGHT);

    return createNewGameBoard(newWidth, newHeight, tileTypes);

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

//==============================================================================
// Private Functions
//==============================================================================

//------------------------------------------------------------------------------
// createNewGameBoard - Tries to create a new gameboard. If it cannot, the
// original GameBoard and GameState are still set.
//------------------------------------------------------------------------------

int MainWindowPresenter::createNewGameBoard(const unsigned int& newWidth, const unsigned int& newHeight, const unsigned int& newTileTypes) {

    GameBoard newBoard(newWidth, newHeight, newTileTypes);
    if(!newBoard.tryNewGame()) {
        return 0;
    }

    reset();
    
    windowData.gameState = GameState::STATE_NOT_STARTED;
    gameBoard = newBoard;

    mainWindow.implGameStateChanged(windowData.gameState);
    
    return 1;
}


//------------------------------------------------------------------------------
// shouldEndGameIfPlaying - Check if the game is playing, and ask the player if
// they want to end it to complete the action they requested
//------------------------------------------------------------------------------

bool MainWindowPresenter::shouldEndGameIfPlaying(const int& LangStrID) {

    if(windowData.gameState != GameState::STATE_GAMEWON && windowData.gameState != GameState::STATE_NO_GAME) {

        const int retVal = mainWindow.implAskYesNoQuestion(GET_LANG_STR(LangStrID), GET_LANG_STR(LangStrID+1));

        if(retVal != MainWindowInterfaceResponses::YES) {
            return false;
        }

    }

    return true;
}

//------------------------------------------------------------------------------
// tryPause - Attempt to pause the game. If it does, it will also update how
// much time the game has been played elapsed before the game was paused.
//------------------------------------------------------------------------------

bool MainWindowPresenter::tryPause() {

    if(windowData.gameState != GameState::STATE_PLAYING) {
        // You can only pause games that are being played.
        return false;
    }

    updateElapsedTime();
    windowData.gameState = GameState::STATE_PAUSED;
    mainWindow.implGameStateChanged(windowData.gameState);

    return true;

}

//------------------------------------------------------------------------------
// tryUnpause - Attempts to unpause the game and resume the how much time the
// game has been played.
//------------------------------------------------------------------------------

bool MainWindowPresenter::tryUnpause() {

    // Cannot Unpause if game is playing or won. (Which all states above PAUSED
    // are.
    if(windowData.gameState > GameState::STATE_PAUSED) {
        
        return false;
    }
   
    milliStartTime = GET_MILLI_COUNT();
    windowData.gameState = GameState::STATE_PLAYING;
    mainWindow.implGameStateChanged(windowData.gameState);

    return true;
}

//------------------------------------------------------------------------------
// updateElapsedTime - Updates how long the game has been played. You must
// ensure that the game is not paused before calling this function.
//------------------------------------------------------------------------------

void MainWindowPresenter::updateElapsedTime() {

    assert(windowData.gameState == GameState::STATE_PLAYING);

    const uint32_t timeNow = GET_MILLI_COUNT();
    milliElapsedTime += (timeNow - milliStartTime);
    milliStartTime = timeNow;

    if(milliElapsedTime > milliPointDelta + 999) {
        
        uint32_t seconds = (milliElapsedTime - milliPointDelta) / 1000;
        
        if(windowData.points < seconds) {
            windowData.points = 0;
        }
        else {
            windowData.points -= static_cast<uint8_t>(seconds);
        }

        milliPointDelta += (seconds * 1000);
    }

}

//==============================================================================
// Things that really shouldn't be here and should be moved at some point.
//==============================================================================

//------------------------------------------------------------------------------
// writeSettings - Writes to an ini file.
//------------------------------------------------------------------------------

bool MainWindowPresenter::writeSettings() {
    wchar_t buffer[4] = {0};

    const GameBoardDimensions boardDimensions = gameBoard.getBoardDimensions();
    
    wsprintf(buffer, L"%d", boardDimensions.width);
    if( !WRITE_INI_STRING(L"settings", L"width", buffer, L".\\ChristmasTiles.ini") ) {
        return false;
    }

    wsprintf(buffer, L"%d", boardDimensions.height);
    if(!WRITE_INI_STRING(L"settings", L"height", buffer, L".\\ChristmasTiles.ini")) {
        return false;
    }
    // TODO: Tileset
    return true;

}

//------------------------------------------------------------------------------
// readSettings - Read settings from INI file
//------------------------------------------------------------------------------

bool MainWindowPresenter::readSettings(GameBoardDimensions& boardDimensions) {

    unsigned int boardWidth = ChristmasTilesConstants::DEFAULT_BOARD_WIDTH;
    unsigned int boardHeight = ChristmasTilesConstants::DEFAULT_BOARD_HEIGHT;
    wchar_t tilesetPath[FILE_PATH_MAX];

    if(!DoesFileExist(L".\\ChristmasTiles.ini")) {
        wsprintf(tilesetPath, L"tileset.bmp");
    }
    else {
        boardWidth = READ_INI_UINT(L"settings", L"width", L".\\ChristmasTiles.ini");
        boardHeight = READ_INI_UINT(L"settings", L"height", L".\\ChristmasTiles.ini");
        READ_INI_STRINGW(L"settings", L"tileset", tilesetPath, FILE_PATH_MAX-1, L".\\ChristmasTiles.ini");
    }

    windowData.pathToTileset = tilesetPath;

    boardDimensions.width = boardWidth;
    boardDimensions.height = boardHeight;

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
        READ_INI_STRINGW(L"scores", keyName, buffer, ScoreTableConstants::MAX_NAME_LENGTH-1, L".\\scores.ini");

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