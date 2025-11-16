#pragma once

#include "../model/gameboard.h"
#include "../stdint_compat.h"

class MainWindowPresenter {

    public:

        MainWindowPresenter() : selectedIndex1(GameBoardConstants::NO_SELECTED_INDEX), selectedIndex2(GameBoardConstants::NO_SELECTED_INDEX) {
            // This is for debug
            gameBoard.tryNewGame();
        }

        int tryFlipTileAtCoodinates(uint8_t& xIndex, uint8_t& yIndex);
        inline void unflipTiles();

        inline const GameBoard& getGameBoard() const { return gameBoard; }

    private:

        uint8_t selectedIndex1;
        uint8_t selectedIndex2;

        GameBoard gameBoard;
};