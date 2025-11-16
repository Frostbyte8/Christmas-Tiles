#include "main_window_presenter.h"

#include <vector>

//==============================================================================
// Public Functions
//==============================================================================

//------------------------------------------------------------------------------
// onClickTile - Call when the tile is clicked
//------------------------------------------------------------------------------

int MainWindowPresenter::tryFlipTileAtCoodinates(uint8_t& xIndex, uint8_t& yIndex) {

    unflipTiles();

    const uint8_t MAX_X = gameBoard.getWidth() - 1;
    const uint8_t MAX_Y = gameBoard.getHeight() - 1;

    // Validate indices

    if(xIndex > MAX_X || yIndex > MAX_Y) {
        return GameBoardFlipErrors::OutOfBounds;
    }

    // Next, make sure the tile has not been flipped
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
            return GameBoardFlipErrors::TilesMatched;

        }

        // Tiles do not match, but keep track of this until the next selection
        selectedIndex2 = yIndex;
        return GameBoardFlipErrors::TilesNotMatched;

    }


    return GameBoardFlipErrors::TileFlipped;
}

//------------------------------------------------------------------------------
// unflipTiles - Unflips tiles is necessary
//------------------------------------------------------------------------------

inline void MainWindowPresenter::unflipTiles() {
    if(selectedIndex2 != GameBoardConstants::NO_SELECTED_INDEX) {
        gameBoard.unfilpTiles(selectedIndex1, selectedIndex2);
    }
}