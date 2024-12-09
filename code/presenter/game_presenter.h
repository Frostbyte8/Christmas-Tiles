#pragma once
#ifndef __GAME_PRESENTER_H__
#define __GAME_PRESENTER_H__

#include "..\stdint_compat.h"
#include "..\interface\main_window_interface.h"

namespace GameConstants {
    enum {
        UNSET_TILE      = 0,
        MIN_WIDTH       = 2,
        MIN_HEIGHT      = 2,
        DEF_WIDTH       = 5,
        DEF_HEIGHT      = 9,
        MAX_WIDTH       = 100,
        MAX_HEIGHT      = 100,
        MAX_TYPES       = 100,
        TILE_UNMATCHED  = 0,
        TILE_MATCHED    = 1,    
        MAX_TIME        = 5999000,
    };
}

namespace InitGameErrors {
    enum {
        SUCCESS,
        OUT_OF_MEMORY,
    };
}

namespace GameState {
    enum {
        FINISHED,
        NOT_STARTED,
        PAUSED,
        PLAYING,
    };
}

struct GameTile {
    GameTile() : matched(0), tileType(0) {}
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

        void tryNewGame();
        inline const bool isGameInited() const;
        uint8_t initGame(const uint8_t& inWidth, const uint8_t& inHeight, const uint8_t& inNumTypes);
        bool tryMatch(const int& index1, const int& index2);
        inline bool validIndex(const int& index) const;

    private:

        inline void findFreeIndex(size_t& index);

        // [TODO]: This will eventually go into a model
        uint8_t width;
        uint8_t height;
        uint8_t numTileTypes;
        uint8_t gameState;
        uint16_t tileCount;
        uint16_t matchesNeeded;
        uint16_t matchesMade;
        uint32_t timeElapsed;
        uint32_t timeStarted;
        GameTile* gameTiles;


        MainWindowInterface* view;
};

#endif //__GAME_PRESENTER_H__
