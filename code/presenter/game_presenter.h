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
        TILE_FLIPPED    = 1,
        TILE_MATCHED    = 2,    
        MAX_TIME        = 5999000,
        TILESET_NOT_SET = 0,
        NO_TILE_SELECTED = -1,
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
    GameTile() : matched(GameConstants::TILE_UNMATCHED), tileType(GameConstants::UNSET_TILE) {}
    int8_t tileType;
    uint8_t matched; // 0 Unflipped, 1, Flipped, 2, Matched
};

class GamePresenter {

    public:

        // Constructors

        GamePresenter(MainWindowInterface* mwi);
        ~GamePresenter();

        // Accessors

        const GameTile* getTiles() const;
        const inline uint8_t& getWidth() const { return width; }
        const inline uint8_t& getHeight() const { return height; }
        const inline uint8_t& getGameState() const { return gameState; }
        //const uint16_t& hasFreeTile() const { return freeTile; }
        //inline const int findFreeTileIndex() const;

        // Public Functions
        const uint32_t getElapsedTime();
        bool tryNewGame(const bool forceNew = false);
        inline const bool isGameInited() const;
        inline const bool isPlaying() const;
        
        
        bool bothFlipped() const;
        void unflip();
        bool tryFlipTile(const unsigned int& index);
        bool tryChangeBoardSize(uint8_t newWidth, uint8_t newHeight);
        void tryTogglePause();


    private:

        void changeBoardSize(uint8_t& newWidth, uint8_t& newHeight, uint8_t& inNumTypes);
        bool validIndex(const unsigned int& index) const;

        // [TODO]: Some of this information will go into a model
        uint8_t width;
        uint8_t height;
        uint8_t numTileTypes;
        uint8_t gameState;

        uint16_t freeTile;
        uint16_t tileCount;
        uint16_t matchesNeeded;
        uint16_t matchesMade;
        uint32_t timeElapsed;
        uint32_t timeStarted;

        int tileIndex[2];
        
        GameTile* gameTiles;
        MainWindowInterface* view;
};

#endif //__GAME_PRESENTER_H__
