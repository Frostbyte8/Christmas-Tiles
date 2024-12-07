#include "game_presenter.h"
#include <math.h> // MSVC does this anyways if you include CMATH
#include <stdexcept>

GamePresenter::GamePresenter() : width(0), height(0), numTileTypes(0), tileCount(0),
                                 matchesNeeded(0), matchesMade(0), gameTiles(NULL) {
}

uint8_t GamePresenter::initGame(const uint8_t& inWidth, const uint8_t& inHeight, const uint8_t& inNumTypes) {

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

    gameTiles = new (std::nothrow) GameTile[tileCount];

    if(!gameTiles) {
        return InitGameErrors::OUT_OF_MEMORY;
    }

    // If the board is not divisible by 2, set the middle tile to the free tile,
    // which is always numTileTypes+1;
    if(tileCount % 2) {
        gameTiles[matchesNeeded].tileType = numTileTypes + 1;
    }

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
