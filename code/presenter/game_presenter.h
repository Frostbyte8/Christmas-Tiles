#pragma once
#ifndef __GAME_PRESENTER_H__
#define __GAME_PRESENTER_H__

#include "..\stdint_compat.h"

namespace GameConstants {
    enum {
        FREE_TILE       = -1,
        MAX_WIDTH       = 100,
        MAX_HEIGHT      = 100,
        MAX_TYPES       = 100,
        TILE_UNMATCHED  = 0,
        TILE_MATCHED    = 1,    
    };
}

struct GameTile {
    int8_t tileType;
    uint8_t matched;
};

class GamePresenter {

    public:
        inline const GameTile* getTiles() const { return gameTiles; }
        inline const uint8_t getWidth() const { return width; }
        inline const uint8_t getHeight() const { return height; }

        GamePresenter(const uint8_t& inWidth, const uint8_t& inHeight, const uint8_t& numTypes);
        ~GamePresenter();

        bool tryMatch(const int& index1, const int& index2);
        inline bool validIndex(const int& index) const;

    private:
        uint8_t width;
        uint8_t height;
        uint8_t numTileTypes;
        uint16_t tileCount;
        uint16_t matchesNeeded;
        uint16_t matchesMade;
        GameTile* gameTiles;
};

#endif //__GAME_PRESENTER_H__
