#include "game_presenter.h"
#include <math.h> // MSVC does this anyways if you include CMATH
#include <time.h>
#include <stdexcept>
#include <cassert>

#define FREE_TILE (numTileTypes + 2)

GamePresenter::GamePresenter(MainWindowInterface* inView) : view(inView),
width(0), height(0),
freeTile(0), numTileTypes(0), tileCount(0), matchesNeeded(0), matchesMade(0),
gameTiles(NULL), timeElapsed(0) {
    tileIndex[0] = GameConstants::NO_TILE_SELECTED;
    tileIndex[1] = GameConstants::NO_TILE_SELECTED;
    assert(view);
}

GamePresenter::~GamePresenter() {
    if(gameTiles) {
        delete[] gameTiles;
        gameTiles = NULL;
    }
}

//=============================================================================
// Accessors
//=============================================================================

//-----------------------------------------------------------------------------
// getTiles - Returns a pointer to a const game tiles structure.
// @return a pointer to a constant GameTile structure
//-----------------------------------------------------------------------------

const GameTile* GamePresenter::getTiles() const {
    return gameTiles;
}

//=============================================================================
// Public Functions
//=============================================================================

//-----------------------------------------------------------------------------
// isGameInited - Checks to see if the game has been initialized.
// @return true if it has, false if it has not
//-----------------------------------------------------------------------------

inline const bool GamePresenter::isGameInited() const {
    return gameTiles != NULL ? true : false;
}

//-----------------------------------------------------------------------------
// changeBoardSettings - Changes either the Width, Height, Number of Tile types,
// or any combination of the three.
// @param uint8_t containing the new width, and also returns the width chosen by the game.
// @param uint8_t containing the new height, and also returns the height chosen by the game
// @param uint8_t containing the number of tile types.
//-----------------------------------------------------------------------------

void GamePresenter::changeBoardSize(uint8_t ioWidth, uint8_t ioHeight, uint8_t inNumTypes) {

    assert(view);
    // TODO: Better Randomness later
    srand(static_cast<unsigned int>(time(NULL)));

    bool tileArrayInvalid = false;

    // Clamp Values
    if(ioWidth > GameConstants::MAX_WIDTH) {
        ioWidth = GameConstants::MAX_WIDTH;
    }
    else if(ioWidth < GameConstants::MIN_WIDTH) {
        ioWidth = GameConstants::MIN_WIDTH;
    }

    if(ioHeight > GameConstants::MAX_HEIGHT) {
        ioHeight = GameConstants::MAX_HEIGHT;
    }
    else if(ioHeight < GameConstants::MIN_HEIGHT) {
        ioHeight = GameConstants::MIN_HEIGHT;
    }

    if(width != ioWidth || height != ioHeight) {
        tileArrayInvalid = true;
        width = ioWidth;
        height = ioHeight;
        tileCount = width * height;
        matchesNeeded = static_cast<uint16_t>(floor(tileCount / 2.f));
        //freeTile = matchesNeeded * 2 == tileCount ? 0 : matchesNeeded;
    }
    
    if(inNumTypes > GameConstants::MAX_TYPES) {
        inNumTypes = GameConstants::MAX_TYPES;
    }
    // TODO: Minimum types.

    // If the user tries to have more than 100 types, tile 101 will be the free tile and the rest will be ignored.

    if(numTileTypes != inNumTypes) {
    }
    numTileTypes = inNumTypes > GameConstants::MAX_TYPES ? GameConstants::MAX_TYPES : inNumTypes;

    // Avoid unnecessary delete
    if(tileArrayInvalid && gameTiles) {
        delete[] gameTiles;
        gameTiles = NULL;
    }

}

//-----------------------------------------------------------------------------
// tryNewGame - Attempts to create a new game.
//-----------------------------------------------------------------------------

void GamePresenter::tryNewGame() {

    if(!tileCount) {
        return;
    }

    timeElapsed = 0;
    timeStarted = 0;
    gameState   = GameState::NOT_STARTED;
    matchesMade = 0;

    if(!gameTiles) {
        gameTiles = new (std::nothrow) GameTile[tileCount];
        if(!gameTiles) {
            return; // [TODO]: Handle error allocating tiles
        }
    }

    // If the board is not divisible by 2, set the middle tile to the free tile.
    if(tileCount % 2) {
        gameTiles[matchesNeeded].tileType = FREE_TILE;
        gameTiles[matchesNeeded].matched = 2;
    }

    size_t lastEmptyIndex = 0;
    for(uint16_t i = 0; i < matchesNeeded; ++i) {

        size_t index[2] = {static_cast<size_t>(rand() % tileCount), static_cast<size_t>(rand() % tileCount)};
        int8_t newType = static_cast<int8_t>(rand() % numTileTypes) + 1;

        // Loop through both new tiles, and find them an unused tile to be set to.

        for(size_t k = 0; k < 2; ++k) {

            if(k != 0) {
                // Avoid setting tile already set.
            }

            if(!gameTiles[index[k]].tileType) {
                // Tile empty, No need to check anymore
                gameTiles[index[k]].tileType = newType;
                continue;
            }

            index[k] = lastEmptyIndex;
            for(;;) {

                if(gameTiles[index[k]].tileType) {
                    // Tile not empty, go to next
                    index[k]++;
                }
                else {

                    // Tile found, and as such, 
                    // No need to start over every iteration, just continue
                    // from the last unfilled tile found.
                    gameTiles[index[k]].tileType = newType;
                    lastEmptyIndex = index[k] + 1;
                    break;
                }
            }
        }

    }
}

bool GamePresenter::tryMatch(const size_t& index1, const size_t& index2) {

    // [TODO]: Add assert to test indices

    if(gameTiles[index1].tileType == gameTiles[index2].tileType) {
        gameTiles[index1].matched = GameConstants::TILE_MATCHED;
        gameTiles[index2].matched = GameConstants::TILE_MATCHED;    
        matchesMade++;
        return true;
    }

    return false;
}

bool GamePresenter::validIndex(const unsigned int& index) const {

    if(index >= tileCount || gameTiles[index].tileType == FREE_TILE) {
        return false;
    }

    return true;
}

bool GamePresenter::bothFlipped() {
    if(tileIndex[0] != GameConstants::NO_TILE_SELECTED && tileIndex[1] != GameConstants::NO_TILE_SELECTED) {
        return true;
    }
    return false;
}

void GamePresenter::unflip() {
    gameTiles[tileIndex[0]].matched = 0;
    gameTiles[tileIndex[1]].matched = 0;
    tileIndex[0] = GameConstants::NO_TILE_SELECTED;
    tileIndex[1] = GameConstants::NO_TILE_SELECTED;
}

bool GamePresenter::tryFlipTile(const unsigned int& index) {

    if(bothFlipped()) {
        unflip();
    }

    if(!validIndex(index) || gameTiles[index].matched) {
        return false;
    }
    
    gameTiles[index].matched = 1;

    if(tileIndex[0] != GameConstants::NO_TILE_SELECTED) {

        if(gameTiles[index].tileType == gameTiles[tileIndex[0]].tileType) {

            gameTiles[tileIndex[0]].matched = 2;
            gameTiles[index].matched = 2;

            tileIndex[0] = GameConstants::NO_TILE_SELECTED;
            tileIndex[1] = GameConstants::NO_TILE_SELECTED;

            matchesMade++;
            if(matchesMade == matchesNeeded) {
                view->gameWon();
            }

        }
        else {
            tileIndex[1] = index;
        }

    }
    else {
        tileIndex[0] = index;
    }

    return true;
}
