#include "game_presenter.h"
#include <math.h> // MSVC does this anyways if you include CMATH
#include <stdexcept>
#include <cassert>

GamePresenter::GamePresenter(MainWindowInterface* inView) : view(inView),
width(0), height(0), numTileTypes(0), tileCount(0), matchesNeeded(0),
matchesMade(0), gameTiles(NULL) {
    assert(view);
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
// initGame - Creates a new game with the given width, height, and number of
// tileTypes.
// @param uint8_t containing the new width.
// @param uint8_t containing the new height.
// @param uint8_t containing the number of tile types.
// @return 0 on success, any other number if an error occurred see InitGameErrors for error codes
//-----------------------------------------------------------------------------

uint8_t GamePresenter::initGame(const uint8_t& inWidth, const uint8_t& inHeight, const uint8_t& inNumTypes) {

    assert(view);

    width = inWidth > GameConstants::MAX_WIDTH ? GameConstants::MAX_WIDTH : inWidth;
    height = inHeight > GameConstants::MAX_HEIGHT ? GameConstants::MAX_HEIGHT : inHeight;

    if(width < GameConstants::MIN_WIDTH) {
        width = GameConstants::MIN_WIDTH;
    }
    
    if(height < GameConstants::MIN_HEIGHT) {
        height = GameConstants::MIN_HEIGHT;
    }

    numTileTypes = inNumTypes > GameConstants::MAX_TYPES ? GameConstants::MAX_TYPES : inNumTypes;

    tileCount = width * height;
    matchesNeeded = static_cast<uint16_t>(floor(tileCount / 2.f));

    if(gameTiles) {
        delete gameTiles;
        gameTiles = NULL;
    }

    gameTiles = new (std::nothrow) GameTile[tileCount];

    if(!gameTiles) {
        return InitGameErrors::OUT_OF_MEMORY;
    }

    // If the board is not divisible by 2, set the middle tile to the free tile,
    // which is always numTileTypes+1;
    if(tileCount % 2) {
        gameTiles[matchesNeeded].tileType = numTileTypes + 1;
    }

    // [DEBUG]
    view->displayMessageBox(0);

    return InitGameErrors::SUCCESS;
}

GamePresenter::~GamePresenter() {
    if(gameTiles) {
        delete gameTiles;
        gameTiles = NULL;
    }
}

bool GamePresenter::tryMatch(const int& index1, const int& index2) {

    // You should ensure that indexes are valid before using this function.

    if(gameTiles[index1].tileType == gameTiles[index2].tileType) {
        gameTiles[index1].matched = GameConstants::TILE_MATCHED;
        gameTiles[index2].matched = GameConstants::TILE_MATCHED;    
        matchesMade++;
        return true;
    }

    return false;
}

inline bool GamePresenter::validIndex(const int& index) const {

    if(index >= tileCount || gameTiles[index].tileType == GameConstants::FREE_TILE) {
        return false;
    }

    return true;
}
