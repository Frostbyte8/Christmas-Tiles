#pragma once

#include "gametile.h"
#include "../stdint_compat.h"
#include <vector>

// Max Dims: 15x15
// TODO look into fast int types and what they do

namespace GameBoardConstants {
    static const uint8_t FREE_TILE_ID = 1; 
    static const uint8_t NO_SELECTED_INDEX = 255;
}

namespace GameBoardFlipErrors {
    static const int AlreadyFlipped = -2;
    static const int OutOfBounds    = -1;
    static const int WasSuccessful  = 0; // < 0 = Success, > 0 = Failure
    static const int TileFlipped    = 1;
    static const int TilesMatched   = 2;
    static const int TilesNotMatched = 3;
}

class GameBoard {

    public:
        GameBoard() : width(5), height(8), selectedIndex(GameBoardConstants::NO_SELECTED_INDEX), selectedIndex2(GameBoardConstants::NO_SELECTED_INDEX) {
            // 10x10 Grid. Even if we don't use 10x10 tiles, the struct is is small,
            // and we might as well take advantage of that.
            tiles.resize(width * height);
        }
        
        bool hasFreeTile() const;
        bool isTileFlipped();
        bool tryNewGame();
        int tryFlipTileAtCoodinates(uint16_t& xPos, uint16_t& yPos, const uint16_t& tileWidth, const uint16_t& tileHeight);

        const std::vector<TileData>& getTiles() const  { return tiles; }
        const uint8_t& getWidth() const { return width; }
        const uint8_t& getHeight() const { return height; }
        void unflipTiles();
        
    private:

        inline void checkAndUnflip();

        uint8_t width;
        uint8_t height;
        //uint8_t freeTileIndex;
        uint8_t numTiles;
        uint8_t matchesNeeded;
        uint8_t matchesDone;
        uint8_t selectedIndex;
        uint8_t selectedIndex2;
        uint8_t numTileTypes;
        std::vector<TileData> tiles;

};