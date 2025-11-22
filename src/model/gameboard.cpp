#include "gameboard.h"
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#ifdef _DEBUG
#include <windows.h>
#endif // _DEBUG

GameBoard::GameBoard(const uint8_t& newWidth, const uint8_t& newHeight, 
const uint8_t& newTileTypes) : matchesNeeded(0) {

    width = newWidth < GameBoardConstants::MAX_WIDTH ? newWidth : GameBoardConstants::MAX_WIDTH;
    height = newHeight < GameBoardConstants::MAX_HEIGHT ? newHeight : GameBoardConstants::MAX_HEIGHT;
    numTileTypes = newTileTypes < GameBoardConstants::MAX_TILE_TYPES ? newTileTypes : GameBoardConstants::MAX_TILE_TYPES;

    tiles.resize(width * height);
}

//------------------------------------------------------------------------------
// tryNewGame - Attempts to start a new game
//------------------------------------------------------------------------------

bool GameBoard::tryNewGame() {

    if(numTileTypes == 0 || width < 2 || height < 2) {
        return false;
    }
    
    // Get the number of tiles this board has
    uint8_t numTiles = (width * height);
    matchesNeeded = numTiles / 2;

    for(unsigned int i = 0; i < numTiles; ++i) {
        tiles[i].ID = 0;
    }
    
    // Determine if this board has a free tile, and if so, set it.
    if(numTiles % 2) {
        tiles[(numTiles-1) / 2].ID = 1; // Free Tile
        tiles[(numTiles-1) / 2].flags = TileFlags::MATCHED;
    }

    uint8_t firstUnfilledIndex = 0;
    uint8_t matchesMade = 0;

    srand(static_cast<unsigned int>(time(NULL)));

    for(unsigned int i = 0 ;; ++i) {

        const uint8_t TILE_ID = (rand() % numTileTypes) + 2;

        // Seek the first tile that does not have data set
        if(tiles[firstUnfilledIndex].ID) {
            firstUnfilledIndex++;
            continue;
        }

        // Fill the tile, and move the next unfilled index to the next index.
        // DEBUG: Just so I can see this memory wise.
        // numTileTypes needs to be used for the ID
        //tiles[firstUnfilledIndex].ID = (matchesMade + 2);
        tiles[firstUnfilledIndex].ID = TILE_ID;
        tiles[firstUnfilledIndex].flags = TileFlags::UNFLIPPED;
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

                // DEBUG: Just so I can see this memory wise.
                // numTileTypes needs to be used for the ID
                //tiles[matchIndex].ID = (matchesMade + 2);
                tiles[matchIndex].ID = TILE_ID;
                tiles[matchIndex].flags = TileFlags::UNFLIPPED;

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

    return true;
   
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

    return true; // TODO: probably do not need this.

}