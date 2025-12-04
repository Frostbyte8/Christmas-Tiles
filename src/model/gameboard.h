#pragma once

#include "gametile.h"
#include "../stdint_compat.h"
#include <vector>

namespace GameBoardConstants {
    static const unsigned int FREE_TILE_ID = 1; 
    static const unsigned int NO_SELECTED_INDEX = 255;
    static const unsigned int MIN_TILE_TYPES = 2;
    static const unsigned int MAX_TILE_TYPES = 100;
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

        GameBoard(const unsigned int& newWidth, const unsigned int& newHeight, const unsigned int& newTileTypes);
        GameBoard();
        // Accessors

        inline const unsigned int& getMatchesNeeded() const { return matchesNeeded; }
        inline const GameBoardDimensions& getBoardDimensions() const { return boardDimensions; }
        inline const std::vector<TileData>& getTiles() const { return tiles; }
        inline const unsigned int& getNumTileTypes() const { return numTileTypes; }

        // Mutators

        void setTileFlipped(const unsigned int& index) {
            tiles[index].flags = TileFlags::FLIPPED;
        }

        void setTilesMatched(const unsigned int& index1, const unsigned int& index2) {
            tiles[index1].flags = TileFlags::MATCHED;
            tiles[index2].flags = TileFlags::MATCHED;
        }      

        void setTilesUnflipped(const unsigned int& index1, const unsigned int& index2) {
            tiles[index1].flags = TileFlags::UNFLIPPED;
            tiles[index2].flags = TileFlags::UNFLIPPED;
        }
        
        // Public Functions

        bool tryNewGame();
        
    private:

        GameBoardDimensions boardDimensions;

        unsigned int numTileTypes;
        unsigned int matchesNeeded;

        std::vector<TileData> tiles;

};