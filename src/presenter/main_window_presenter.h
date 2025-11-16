#pragma once

#include "../model/gameboard.h"
#include "../stdint_compat.h"

class MainWindowPresenter {

    public:

        MainWindowPresenter() : selectedIndex1(GameBoardConstants::NO_SELECTED_INDEX), 
            selectedIndex2(GameBoardConstants::NO_SELECTED_INDEX), matchesMade(0),
            milliStartTime(0), milliElapsedTime(0), isPaused(true) {
            // This is for debug
            gameBoard.tryNewGame(16);
        }

        const uint32_t& getElapsedTime();

        int tryFlipTileAtCoodinates(uint8_t& xIndex, uint8_t& yIndex);
        inline void unflipTiles();

        inline const GameBoard& getGameBoard() const { return gameBoard; }

        bool tryPause();
        bool tryUnpause();

    private:

        void        updateElapsedTime();

        bool        isPaused;
        uint8_t     selectedIndex1;
        uint8_t     selectedIndex2;
        uint8_t     matchesMade;
        uint32_t    milliStartTime;
        uint32_t    milliElapsedTime;

        GameBoard gameBoard;
};