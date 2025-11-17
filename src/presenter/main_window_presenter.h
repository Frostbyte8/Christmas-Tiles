#pragma once

#include "../model/gameboard.h"
#include "../stdint_compat.h"
#include "../interface/main_window_interface.h"

namespace GameState {
    enum GameState {
        STATE_NOT_STARTED       = -1,
        STATE_PAUSED            = 0,
        STATE_GAMEWON           = 1,
        STATE_PLAYING           = 2,
        
    };
}

class MainWindowPresenter {

    public:

        MainWindowPresenter() : selectedIndex1(GameBoardConstants::NO_SELECTED_INDEX), 
            selectedIndex2(GameBoardConstants::NO_SELECTED_INDEX), matchesMade(0),
            milliStartTime(0), milliElapsedTime(0), score(0), points(50), gameState(GameState::STATE_NOT_STARTED), milliPointDelta(0) {
            // This is for debug
            //gameBoard.tryNewGame(16);
        }

        const uint32_t& getElapsedTime();
        const uint32_t& getScore() const { return score; }
        const uint8_t& getPoints() const { return points; }

        int tryFlipTileAtCoodinates(uint8_t& xIndex, uint8_t& yIndex);
        inline void unflipTiles();

        inline const GameBoard& getGameBoard() const { return gameBoard; }

        bool tryTogglePause();
        bool tryNewGame(const uint8_t& numTileTypes);

        void setMainWindow(MainWindowInterface* mwi) {
            mainWindow = mwi;
        }

        /*
        void test() {
            mainWindow->implDisplayTestMessage();
        }
        */

    private:

        MainWindowInterface *mainWindow;

        bool tryPause();
        bool tryUnpause();
        void        updateElapsedTime();

        int         gameState;
        uint8_t     selectedIndex1;
        uint8_t     selectedIndex2;
        uint8_t     matchesMade;
        uint32_t    milliStartTime;
        uint32_t    milliElapsedTime;
        uint32_t    milliPointDelta;

        uint8_t     points;
        uint32_t    score;

        GameBoard gameBoard;
};