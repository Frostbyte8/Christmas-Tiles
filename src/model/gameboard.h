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

        GameBoard() : width(5), height(8) {
            // 10x10 Grid. Even if we don't use 10x10 tiles, the struct is is small,
            // and we might as well take advantage of that.
            tiles.resize(width * height);
        }

        // Accessors

        inline const uint8_t& getMatchesNeeded() const { return matchesNeeded; }
        inline const uint8_t& getWidth() const { return width; }
        inline const uint8_t& getHeight() const { return height; }
        inline const std::vector<TileData>& getTiles() const { return tiles; }

        // Mutators

        void setTileFlipped(const uint8_t& index) {
            tiles[index].flags = TileFlags::FLIPPED;
        }

        void setTilesMatched(const uint8_t& index1, const uint8_t& index2) {
            tiles[index1].flags = TileFlags::MATCHED;
            tiles[index2].flags = TileFlags::MATCHED;
        }      

        void setTilesUnflipped(const uint8_t& index1, const uint8_t& index2) {
            tiles[index1].flags = TileFlags::UNFLIPPED;
            tiles[index2].flags = TileFlags::UNFLIPPED;
        }

         // Public Functions

        bool tryNewGame(const uint8_t& numTileTypes);
        
    private:

        uint8_t width;
        uint8_t height;
        uint8_t numTiles;
        uint8_t matchesNeeded;

        std::vector<TileData> tiles;

};