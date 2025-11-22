#include "main_window_presenter.h"

#include <vector>
#include <assert.h>

#ifdef _WIN32

#include <windows.h>

// GET_MILLI_COUNT - Must return the number of seconds elapsed since the system
// was started. On Win32 this just calls GetTickCount

#define GET_MILLI_COUNT GetTickCount

#endif // _WIN32

#include "../language_table.h"


//==============================================================================
// Namespaces / Enums / Constants
//==============================================================================

static const uint32_t MAX_SCORE = 999999999;

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
            if(score > MAX_SCORE) {
                score = MAX_SCORE;
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