#pragma once

#include "gametile.h"
#include "../stdint_compat.h"
#include <vector>

namespace GameBoardConstants {
    static const uint8_t FREE_TILE_ID = 1; 
    static const uint8_t NO_SELECTED_INDEX = 255;
    static const uint8_t MIN_TILE_TYPES = 3;
    static const uint8_t MAX_TILE_TYPES = 100;
    static const unsigned int MAX_WIDTH = 15;
    static const unsigned int MAX_HEIGHT = 15;
    static const unsigned int MIN_WIDTH = 2;
    static const unsigned int MIN_HEIGHT = 2;
    static const unsigned int DEFAULT_WIDTH = 5;
    static const unsigned int DEFAULT_HEIGHT = 9;
}

struct GameBoardDimensions {
    unsigned int width;
    unsigned int height;
};

class GameBoard {

    public:

        GameBoard(const unsigned int& newWidth, const unsigned int& newHeight, const uint8_t& newTileTypes);
        GameBoard();
        // Accessors

        inline const uint8_t& getMatchesNeeded() const { return matchesNeeded; }
        inline const GameBoardDimensions& getBoardDimensions() const { return boardDimensions; }
        inline const std::vector<TileData>& getTiles() const { return tiles; }
        inline const uint8_t& getNumTileTypes() const { return numTileTypes; }

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

        bool tryNewGame();
        
    private:

        GameBoardDimensions boardDimensions;

        uint8_t numTileTypes;
        uint8_t matchesNeeded;

        std::vector<TileData> tiles;

};