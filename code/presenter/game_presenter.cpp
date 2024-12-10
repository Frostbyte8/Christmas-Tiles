#include "game_presenter.h"
#include <math.h> // MSVC does this anyways if you include CMATH
#include <time.h>
#include <stdexcept>
#include <cassert>

#define FREE_TILE (numTileTypes + 1)

GamePresenter::GamePresenter(MainWindowInterface* inView) : view(inView),
width(0), height(0),
numTileTypes(0), tileCount(0), matchesNeeded(0), matchesMade(0),
gameTiles(NULL), timeElapsed(0) {
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
// getHeight - Returns the height of the current game board
// @return a uint8_t containing the width of the game board
//-----------------------------------------------------------------------------

inline const uint8_t GamePresenter::getHeight() const {
    return height;
}

//-----------------------------------------------------------------------------
// getTiles - Returns a pointer to a const game tiles structure.
// @return a pointer to a constant GameTile structure
//-----------------------------------------------------------------------------

inline const GameTile* GamePresenter::getTiles() const {
    return gameTiles;
}

//-----------------------------------------------------------------------------
// getWidth - Returns the width of the current game board
// @return a uint8_t containing the width of the game board
//-----------------------------------------------------------------------------

inline const uint8_t GamePresenter::getWidth() const {
    return width;
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

void GamePresenter::changeBoardSize(uint8_t& ioWidth, uint8_t& ioHeight, uint8_t inNumTypes) {

    assert(view);
    // TODO: Better Randomness later
    srand(time(NULL));

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
        gameTiles[matchesNeeded].matched = 1;
    }

    size_t lastEmptyIndex = 0;
    for(uint16_t i = 0; i < matchesNeeded; ++i) {

        size_t index[2] = {static_cast<size_t>(rand() % tileCount), static_cast<size_t>(rand() % tileCount)};
        int8_t newType = static_cast<int8_t>(rand() % numTileTypes) + 1;

        // Loop through both new tiles, and find them an unused tile to be set to.

        for(size_t k = 0; k < 2; ++k) {

            if(gameTiles[index[k]].tileType) {
                // No need to check
                continue;
            }

            index[k] = lastEmptyIndex;
            for(;;) {
                if(gameTiles[index[k]].tileType) {
                    index[k]++;
                }
                else {
                    // No need to start over every iteration, just continue
                    // from the last unfilled tile found.
                    lastEmptyIndex = index[k] + 1;
                    break;
                }
            }
        }

        gameTiles[index[0]].tileType = newType;
        gameTiles[index[1]].tileType = newType;

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

inline bool GamePresenter::validIndex(const int& index) const {

    if(index >= tileCount || gameTiles[index].tileType == FREE_TILE) {
        return false;
    }

    return true;
}
