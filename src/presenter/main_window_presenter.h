#pragma once

#include "../model/gameboard.h"
#include "../stdint_compat.h"
#include "../interface/main_window_interface.h"
#include "../model/scores.h"

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

namespace GameBoardFlipErrors {
    static const int GameAlreadyWon = -4;
    static const int CannotUnpause = -3;
    static const int AlreadyFlipped = -2;
    static const int OutOfBounds    = -1;
    static const int WasSuccessful  = 0; // < 0 = Success, > 0 = Failure
    static const int TileFlipped    = 1;
    static const int TilesMatched   = 2;
    static const int TilesNotMatched = 3;
}

struct MainWindowData {
    int             gameState;
    unsigned int    points;
    uint32_t        score;
};

class MainWindowPresenter {

    public:

        MainWindowPresenter(MainWindowInterface& inMWI);

        const uint32_t& getElapsedTime();
        //const uint32_t& getScore() const { return windowData.score; }
        //const int& getPoints() const { return windowData.points; }
        const MainWindowData& getMainWindowData() const { return windowData; }

        int tryFlipTileAtCoodinates(unsigned int& xIndex, unsigned int& yIndex);
        bool tryAddScore(wchar_t* name, const size_t& index);
        inline void unflipTiles();

        inline const GameBoard& getGameBoard() const { return gameBoard; }
        inline const int& getGameState() const { return windowData.gameState; }

        bool requestNewGame();
        bool tryTogglePause();
        bool tryUpdateGameBoard(unsigned int newWidth, unsigned int newHeight, uint8_t tileTypes);
        __forceinline const ScoreTable& getScoreTable() const { return scoreTable; }

        /*
        void setMainWindow(MainWindowInterface* mwi) {
            mainWindow = mwi;
        }
        */

        // TODO: At some point this will all be moved in to the models they
        // belong in.
        bool readScores();
        bool writeSettings();
        bool writeScores();

    private:

        __forceinline void reset();

        bool tryNewGame();
        bool tryPause();
        bool tryUnpause();
        void updateElapsedTime();

        
        /*
        uint8_t     points;
        uint32_t    score;
        uint32_t    milliElapsedTime;
        int         gameState;
        */

        int                 selectedIndex1;
        int                 selectedIndex2;
        unsigned int        matchesMade;
        uint32_t            milliStartTime;
        uint32_t            milliElapsedTime;
        uint32_t            milliPointDelta;      

        MainWindowData          windowData;
        GameBoard               gameBoard;
        ScoreTable              scoreTable;
        MainWindowInterface&    mainWindow;
};
