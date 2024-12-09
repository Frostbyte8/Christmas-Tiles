#pragma once
#ifndef __GAME_PRESENTER_H__
#define __GAME_PRESENTER_H__

#include "..\stdint_compat.h"
#include "..\interface\main_window_interface.h"

namespace GameConstants {
    enum {
        FREE_TILE       = -1,
        MIN_WIDTH       = 2,
        MIN_HEIGHT      = 2,
        MAX_WIDTH       = 100,
        MAX_HEIGHT      = 100,
        MAX_TYPES       = 100,
        TILE_UNMATCHED  = 0,
        TILE_MATCHED    = 1,    
    };
}

namespace InitGameErrors {
    enum {
        SUCCESS,
        OUT_OF_MEMORY,
    };
}

struct GameTile {
    int8_t tileType;
    uint8_t matched;
};

class GamePresenter {

    public:

        // Constructors

        GamePresenter(MainWindowInterface* mwi);
        ~GamePresenter();

        // Accessors

        inline const GameTile* getTiles() const;
        inline const uint8_t getWidth() const;
        inline const uint8_t getHeight() const;

        // Public Functions

        inline const bool isGameInited() const;
        uint8_t initGame(const uint8_t& inWidth, const uint8_t& inHeight, const uint8_t& inNumTypes);
        bool tryMatch(const int& index1, const int& index2);
        inline bool validIndex(const int& index) const;

    private:
        // Normally this would go into a model, but since it is so small, I've
        // opted not to at this time.
        uint8_t width;
        uint8_t height;
        uint8_t numTileTypes;
        uint16_t tileCount;
        uint16_t matchesNeeded;
        uint16_t matchesMade;
        GameTile* gameTiles;

        MainWindowInterface* view;
};

#endif //__GAME_PRESENTER_H__
