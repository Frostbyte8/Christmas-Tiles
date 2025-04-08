#include "game_presenter.h"
#include <math.h> // MSVC does this anyways if you include CMATH
#include <time.h>
#include <stdexcept>
#include <cassert>

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

#define FREE_TILE (numTileTypes + 2)

__forceinline uint32_t getMSCount() {
#ifdef _WIN32
    return GetTickCount();
#endif // _WIN32
}

//=============================================================================
// Constructor
//=============================================================================

GamePresenter::GamePresenter(MainWindowInterface* inView) : view(inView),
width(0), height(0), gameState(GameState::NOT_STARTED),
freeTile(0), numTileTypes(0), tileCount(0), matchesNeeded(0), matchesMade(0),
gameTiles(NULL), timeElapsed(0) {
    tileIndex[0] = GameConstants::NO_TILE_SELECTED;
    tileIndex[1] = GameConstants::NO_TILE_SELECTED;
    assert(view);
}

//=============================================================================
// Destructor
//=============================================================================

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

//-----------------------------------------------------------------------------
// getElapsedTime - get up to date information about how long the game has been
// played for. It will update the time before returning it so it is accurate.
// @return uint32_t containing the number of milliseconds elapsed.
//-----------------------------------------------------------------------------

const uint32_t GamePresenter::getElapsedTime() {

    if(gameState == GameState::PLAYING) {
        uint32_t tempTick = getMSCount();
        timeElapsed += tempTick - timeStarted;
        timeStarted = tempTick;
    }

    return timeElapsed;
}

//=============================================================================
// Public Functions
//=============================================================================

//-----------------------------------------------------------------------------
// bothFlipped - Test if two tiles have been flipped.
// @return true if they have, false if they have not
//-----------------------------------------------------------------------------

bool GamePresenter::bothFlipped() const {
    if(tileIndex[0] != GameConstants::NO_TILE_SELECTED &&
       tileIndex[1] != GameConstants::NO_TILE_SELECTED) {
        return true;
    }
    return false;
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

    numTileTypes = inNumTypes > GameConstants::MAX_TYPES ? 
                                GameConstants::MAX_TYPES :
                                inNumTypes;

    // Avoid unnecessary delete
    if(tileArrayInvalid && gameTiles) {
        delete[] gameTiles;
        gameTiles = NULL;
    }

}

//-----------------------------------------------------------------------------
// isGameInited - Checks to see if the game has been initialized.
// @return true if it has, false if it has not
//-----------------------------------------------------------------------------

inline const bool GamePresenter::isGameInited() const {
    return gameTiles != NULL ? true : false;
}

//-----------------------------------------------------------------------------
// isPlaying - Checks if the game is currently being played
// @return true if it is, false if it is nto.
//-----------------------------------------------------------------------------

inline const bool GamePresenter::isPlaying() const {
    if(gameState == GameState::PLAYING || gameState == GameState::PAUSED) {
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
// tryFlipTile - Attempts to flip the tile at the index given.
// @param unsigned int containing the tile index to try flipping
// @return true if the tile was flipped, false if it could not be flipped
//-----------------------------------------------------------------------------

bool GamePresenter::tryFlipTile(const unsigned int& index) {

    if(!isGameInited()) {
        return false;
    }
    else if(gameState == GameState::NOT_STARTED) {
        timeStarted = getMSCount();
        timeElapsed = 0;
        gameState = GameState::PLAYING;
    }
    else if(gameState == GameState::FINISHED) {
        return false;
    }

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
                getElapsedTime();
                gameState = GameState::FINISHED;
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

//-----------------------------------------------------------------------------
// tryNewGame - Attempts to create a new game.
//-----------------------------------------------------------------------------

bool GamePresenter::tryNewGame(const bool forceNew) {

    if(!forceNew) {
        if(gameState != GameState::FINISHED && gameState != GameState::NOT_STARTED) {
            if(view->askQuestion("This game is still in progress. Are you sure you want to start a new game?", "Are you sure?", MessageBoxTypes::YESNOCANCEL) != MessageBoxReponses::YES) {
                return false;
            }
        }
    }

    // TODO: Need another state
    gameState   = GameState::NOT_STARTED;

    if(!tileCount) {
        return false;
    }

    timeElapsed = 0;
    timeStarted = 0;
    matchesMade = 0;

    if(!gameTiles) {
        gameTiles = new (std::nothrow) GameTile[tileCount];
        if(!gameTiles) {
            return false; // [TODO]: Handle error allocating tiles, as well as another game state
        }
    }
    else {
        memset(gameTiles, 0, sizeof(GameTile) * tileCount);
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

            if(!gameTiles[index[k]].tileType) {
                // Tile empty, No need to check anymore
                gameTiles[index[k]].tileType = newType;
                continue;
            }

            // Continue on at the last empty index we are aware of.
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

    tileIndex[1] = tileIndex[0] = GameConstants::NO_TILE_SELECTED;

    return true;
}

//-----------------------------------------------------------------------------
// unflip - Unflips tiles
//-----------------------------------------------------------------------------

void GamePresenter::unflip() {
    gameTiles[tileIndex[0]].matched = 0;
    gameTiles[tileIndex[1]].matched = 0;
    tileIndex[0] = GameConstants::NO_TILE_SELECTED;
    tileIndex[1] = GameConstants::NO_TILE_SELECTED;
}

//=============================================================================
// Private Functions
//=============================================================================

//-----------------------------------------------------------------------------
// validIndex - Checks to see if the given index is valid tile index.
// @param an unsigned int containing the index of the tile to test
// @return true if it is, false if it isn't.
//-----------------------------------------------------------------------------

bool GamePresenter::validIndex(const unsigned int& index) const {

    if(index >= tileCount || gameTiles[index].tileType == FREE_TILE) {
        return false;
    }

    return true;
}
