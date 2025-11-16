#include "gameboard.h"
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#ifdef _DEBUG
#include <windows.h>
#endif // _DEBUG

//------------------------------------------------------------------------------
// tryNewGame - Attempts to start a new game
//------------------------------------------------------------------------------

bool GameBoard::tryNewGame() {
    
    // Get the number of tiles this board has
    numTiles = (width * height);
    matchesNeeded = numTiles / 2;

    for(unsigned int i = 0; i < numTiles; ++i) {
        tiles[i].ID = 0;
    }
    
    // Determine if this board has a free tile, and if so, set it.
    if(numTiles % 2) {
        tiles[(numTiles-1) / 2].ID = 1; // Free Tile
    }

    uint8_t firstUnfilledIndex = 0;
    uint8_t matchesMade = 0;

        srand(static_cast<unsigned int>(time(NULL)));

    for(unsigned int i = 0 ;; ++i) {

        // Seek the first tile that does not have data set
        if(tiles[firstUnfilledIndex].ID) {
            firstUnfilledIndex++;
            continue;
        }

        // Fill the tile, and move the next unfilled index to the next index.
        tiles[firstUnfilledIndex].ID = (matchesMade + 2); // DEBUG: Just so I can see this memory wise.
        firstUnfilledIndex++;

        // Select Random Tile for matching tile

        uint8_t matchIndex = (firstUnfilledIndex) + (rand() % (numTiles - firstUnfilledIndex)) - 1;


        // Now search for the first empty tile, which may even be the one we just
        // generated.

        bool shouldUpdateUnfilled = false;

        for(unsigned int k = 0; k < numTiles; ++k) {

            if(tiles[matchIndex].ID) {
                // Tile already filled, so try the next tile.
                matchIndex++;
            }
            else {

                tiles[matchIndex].ID = (matchesMade + 2); // DEBUG: Just so I can see this memory wise.

                // If we wrapped around, we'll update the unfilled index too.
                if(shouldUpdateUnfilled) {
                    firstUnfilledIndex = matchIndex + 1;
                }
                break;
            }

            // Check and see if the next match Index we are looking at will go
            // out of bounds
            if(matchIndex == numTiles) {
                // Instead of going back to 0, start at the first unfilled
                // index.
                matchIndex = firstUnfilledIndex;
                shouldUpdateUnfilled = true;
            }

        }

        matchesMade++;

        if(matchesMade == matchesNeeded) {
            break;
        }

    }

    assert(matchesMade == matchesNeeded);


#ifdef _DEBUG
   
    wchar_t outStr[5] = {0};

    uint8_t k = 0;

    for(uint8_t i = 0; i < numTiles; ++i) {

        wsprintf(outStr, L"%02d \0", tiles[i].ID);

        ++k;
        OutputDebugStr(outStr);
        if(k == width) {
            k = 0;
            OutputDebugStr(L"\n");
        }

    }


#endif // _DEBUG

    return true;

}

/*

void GameBoard::unflipTiles() {

    if(selectedIndex != GameBoardConstants::NO_SELECTED_INDEX) {
        tiles[selectedIndex].flags = TileFlags::UNFLIPPED;
        selectedIndex = GameBoardConstants::NO_SELECTED_INDEX;
    }

    if(selectedIndex2 != GameBoardConstants::NO_SELECTED_INDEX) {
        tiles[selectedIndex2].flags = TileFlags::UNFLIPPED;
        selectedIndex2 = GameBoardConstants::NO_SELECTED_INDEX;
    }

}

//------------------------------------------------------------------------------
// checkAndUnflip - Checks to see if two tiles are flipped over, and should be
// reset. It assumes that if selectedIndex2 is set, that two tiles a flipped.
//------------------------------------------------------------------------------

inline void GameBoard::checkAndUnflip() {

    if(selectedIndex2 != GameBoardConstants::NO_SELECTED_INDEX) {
        tiles[selectedIndex].flags = TileFlags::UNFLIPPED;
        tiles[selectedIndex2].flags = TileFlags::UNFLIPPED;

        selectedIndex = GameBoardConstants::NO_SELECTED_INDEX;
        selectedIndex2 = GameBoardConstants::NO_SELECTED_INDEX;
    }
}

//------------------------------------------------------------------------------
// tryFlipTileAtCoodinates - With the Coordinates and Tile Dimensions given,
// attempt to flip the tile there if it exists.
//------------------------------------------------------------------------------


int GameBoard::tryFlipTileAtCoodinates(uint16_t& xPos, uint16_t& yPos, const uint16_t& tileWidth, const uint16_t& tileHeight) {

    // Check to see if we have to unflip anything
    checkAndUnflip();

    // Now convert the coordinates to an index

    const uint16_t MAX_X = (width * tileWidth) - 1;
    const uint16_t MAX_Y = (height * tileHeight) - 1;

    // Make sure the tile is within range

    if(xPos > MAX_X || yPos > MAX_Y) {
        return GameBoardFlipErrors::OutOfBounds;
    }

    // Convert Coordinates to Index

    xPos = xPos / tileWidth;
    yPos = ((yPos / 32) * width) + xPos;

    // Make sure the index is within range, and that the tile has not yet
    // be flipped.

    if(yPos > numTiles-1) {
        return GameBoardFlipErrors::OutOfBounds;
    }

    // Check if the tile has not been flipped already, and if it has not,
    // attempt to flip it

    if(tiles[yPos].flags) {
        return GameBoardFlipErrors::AlreadyFlipped;
    }   

    tiles[yPos].flags = TileFlags::FLIPPED;

    if(selectedIndex == GameBoardConstants::NO_SELECTED_INDEX) {
        selectedIndex = static_cast<uint8_t>(yPos);
    }
    else {

        // Do these tiles match?

        if(tiles[selectedIndex].ID == tiles[yPos].ID) {
            tiles[selectedIndex].flags = TileFlags::MATCHED;
            tiles[yPos].flags = TileFlags::MATCHED;
            selectedIndex = GameBoardConstants::NO_SELECTED_INDEX;
            return GameBoardFlipErrors::TilesMatched;
        }

        // Tiles do not match.
        selectedIndex2 = yPos;

        return GameBoardFlipErrors::TilesNotMatched;
    }


    return GameBoardFlipErrors::TileFlipped;
}
*/