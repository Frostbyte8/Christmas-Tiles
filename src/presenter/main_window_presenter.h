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

namespace WindowPresenterConstants {
    static const uint8_t IGNORE_WIDTH       = 255;
    static const uint8_t IGNORE_HEIGHT      = 255;
    static const uint8_t IGNORE_NUMTILES    = 255;
}

class MainWindowPresenter {

    public:

        MainWindowPresenter();

        const uint32_t& getElapsedTime();
        const uint32_t& getScore() const { return score; }
        const uint8_t& getPoints() const { return points; }

        int tryFlipTileAtCoodinates(uint8_t& xIndex, uint8_t& yIndex);
        inline void unflipTiles();

        inline const GameBoard& getGameBoard() const { return gameBoard; }

        bool tryTogglePause();
        bool tryNewGame();
        bool tryUpdateGameBoard(uint8_t& newWidth, uint8_t& newHeight, uint8_t& tileTypes);
        bool updateTileTypes(const uint8_t& tileTypes);
        bool updateBoardSize(const uint8_t& newWidth, const uint8_t& newHeight, const int defaultSize = 0);

        // TODO: The constructor is the only thing that needs this?
        void setMainWindow(MainWindowInterface* mwi) {
            mainWindow = mwi;
        }
    private:

        __forceinline void reset();

        MainWindowInterface *mainWindow;

        bool tryPause();
        bool tryUnpause();
        void updateElapsedTime();


        uint8_t     points;
        uint8_t     selectedIndex1;
        uint8_t     selectedIndex2;
        uint8_t     matchesMade;
        
        uint32_t    milliStartTime;
        uint32_t    milliElapsedTime;
        uint32_t    milliPointDelta;
        uint32_t    score;

        int         gameState;

        GameBoard   gameBoard;
};