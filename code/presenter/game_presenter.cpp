#include "game_presenter.h"
#include <math.h> // MSVC does this anyways if you include CMATH
#include <time.h>
#include <stdexcept>
#include <cassert>

#define FREE_TILE (numTileTypes + 1)

GamePresenter::GamePresenter(MainWindowInterface* inView) : view(inView),
width(GameConstants::DEF_WIDTH), height(GameConstants::DEF_HEIGHT),
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
// initGame 
// TODO: Figure out what belongs here, and what belongs in tryNewGame
// @param uint8_t containing the new width.
// @param uint8_t containing the new height.
// @param uint8_t containing the number of tile types.
// @return 0 on success, any other number if an error occurred see InitGameErrors for error codes
//-----------------------------------------------------------------------------

uint8_t GamePresenter::initGame(const uint8_t& inWidth, const uint8_t& inHeight, const uint8_t& inNumTypes) {

    assert(view);
    // TODO: Better Randomness later
    srand(time(NULL));

    width = inWidth > GameConstants::MAX_WIDTH ? GameConstants::MAX_WIDTH : inWidth;
    height = inHeight > GameConstants::MAX_HEIGHT ? GameConstants::MAX_HEIGHT : inHeight;

    if(width < GameConstants::MIN_WIDTH) {
        width = GameConstants::MIN_WIDTH;
    }
    
    if(height < GameConstants::MIN_HEIGHT) {
        height = GameConstants::MIN_HEIGHT;
    }

    numTileTypes = inNumTypes > GameConstants::MAX_TYPES ? GameConstants::MAX_TYPES : inNumTypes;

    // If the board is not divisible by 2, set the middle tile to the free tile,
    // which is always numTileTypes+1;


    return InitGameErrors::SUCCESS;
}

//-----------------------------------------------------------------------------
// tryNewGame - Attempts to create a new game.
//-----------------------------------------------------------------------------

void GamePresenter::tryNewGame() {

    assert(width >= GameConstants::MIN_WIDTH);
    assert(height >= GameConstants::MIN_HEIGHT);

    timeElapsed = 0;
    timeStarted = 0;
    gameState = GameState::NOT_STARTED;
    matchesMade = 0;

    tileCount = width * height;
    matchesNeeded = static_cast<uint16_t>(floor(tileCount / 2.f));

    if(!gameTiles) {
        gameTiles = new (std::nothrow) GameTile[tileCount];
        if(!gameTiles) {
            return; // [TODO]: Handle error allocating tiles
        }
    }

    // Locate possible free tile
    if(tileCount % 2) {
        gameTiles[matchesNeeded].tileType = numTileTypes + 1;
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

bool GamePresenter::tryMatch(const int& index1, const int& index2) {

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
