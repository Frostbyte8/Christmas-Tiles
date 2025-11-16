#include "main_window_presenter.h"

#include <vector>
#include <assert.h>

#ifdef _WIN32
#include <windows.h>
#define GET_MILLI_COUNT GetTickCount
#endif // _WIN32

//==============================================================================
// Public Functions
//==============================================================================

//------------------------------------------------------------------------------
// onClickTile - Call when the tile is clicked
//------------------------------------------------------------------------------

int MainWindowPresenter::tryFlipTileAtCoodinates(uint8_t& xIndex, uint8_t& yIndex) {

    if(isPaused) {
        if(!tryUnpause()) {
            return GameBoardFlipErrors::AlreadyFlipped; // TODO: New error for pausing
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

            return GameBoardFlipErrors::TilesMatched;

        }

        // Tiles do not match, but keep track of this until the next selection
        selectedIndex2 = yIndex;
        return GameBoardFlipErrors::TilesNotMatched;

    }

    return GameBoardFlipErrors::TileFlipped;

}

//------------------------------------------------------------------------------
// tryUnpause - Attempts to unpause the game and resume the timer
//------------------------------------------------------------------------------

bool MainWindowPresenter::tryUnpause() {

    if(!isPaused) {
        return false;
    }

    assert(matchesMade <= gameBoard.getMatchesNeeded());

    if(matchesMade == gameBoard.getMatchesNeeded()) {
        return false;
    }
    
    milliStartTime = GET_MILLI_COUNT();
    isPaused = false;

    return true;
}

bool MainWindowPresenter::tryPause() {

    if(isPaused) {
        return false;
    }

    assert(matchesMade <= gameBoard.getMatchesNeeded());

    if(matchesMade == gameBoard.getMatchesNeeded()) {
        return false;
    }

    updateElapsedTime();
    isPaused = true;

    return true;

}

const uint32_t& MainWindowPresenter::getElapsedTime() {

    if(!isPaused) {
        updateElapsedTime();
    }

    return milliElapsedTime;
}


void MainWindowPresenter::updateElapsedTime() {

    assert(isPaused == false);

    const uint32_t timeNow = GET_MILLI_COUNT();
    milliElapsedTime += (timeNow - milliStartTime);
    milliStartTime = timeNow;

}

//------------------------------------------------------------------------------
// unflipTiles - Unflips tiles is necessary
//------------------------------------------------------------------------------

inline void MainWindowPresenter::unflipTiles() {

    if(selectedIndex2 != GameBoardConstants::NO_SELECTED_INDEX) {
        
        assert(selectedIndex1 != GameBoardConstants::NO_SELECTED_INDEX);

        gameBoard.setTilesUnflipped(selectedIndex1, selectedIndex2);
        selectedIndex1 = GameBoardConstants::NO_SELECTED_INDEX;
        selectedIndex2 = GameBoardConstants::NO_SELECTED_INDEX;

    }

}