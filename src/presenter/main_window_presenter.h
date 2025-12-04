#pragma once

#include "../model/gameboard.h"
#include "../stdint_compat.h"
#include "../interface/main_window_interface.h"
#include "../model/scores.h"

namespace GameState {
    enum GameState {
        STATE_NO_GAME           = 0,
        STATE_NOT_STARTED       = 1,
        STATE_PAUSED            = 2,
        STATE_GAMEWON           = 3,
        STATE_PLAYING           = 4,
    };
}

namespace WindowPresenterConstants {
    static const unsigned int IGNORE_WIDTH       = 255;
    static const unsigned int IGNORE_HEIGHT      = 255;
    static const unsigned int IGNORE_NUMTILES    = 255;
}

namespace GameBoardFlipErrors {
    static const int GameNotCreated = -5;
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
    wchar_t*        pathToTileset;
};

class MainWindowPresenter {

    public:

        MainWindowPresenter();

        ~MainWindowPresenter() {
            if(windowData.pathToTileset) {
                free(windowData.pathToTileset);
                windowData.pathToTileset = NULL;
            }
        }

        // Accessors

        const uint32_t& getElapsedTime();
        __forceinline const GameBoard& getGameBoard() const { return gameBoard; }
        __forceinline const int& getGameState() const { return windowData.gameState; }
        __forceinline const MainWindowData& getMainWindowData() const { return windowData; }
        __forceinline const ScoreTable& getScoreTable() const { return scoreTable; }

        // Mutators

        void setMainWindowInterface(MainWindowInterface* inMWI) {
            mainWindow = inMWI;
        }
        
        // Public Functions
        bool changeTilesetPath(const wchar_t* newPath);
        const size_t getScorePosition() const;
        int requestNewGame();
        bool shouldEndGameIfPlaying(const int& LangIDTitle);
        bool tryAddScore(wchar_t* name, const size_t& index);
        int tryFlipTileAtCoodinates(unsigned int& xIndex, unsigned int& yIndex);
        bool tryTogglePause();
        int tryUpdateGameBoard(unsigned int& newWidth, unsigned int& newHeight, unsigned int tileTypes, const bool dontWarn);
        inline void unflipTiles();
        
        // Public File I/O

        bool readScores();
        bool readSettings(GameBoardDimensions& boardDimensions);
        bool writeScores();
        bool writeSettings();

    private:
        MainWindowPresenter(const MainWindowPresenter&);
        MainWindowPresenter& operator=(const MainWindowPresenter&);
        __forceinline void reset();

        int createNewGameBoard(const unsigned int& newWidth, const unsigned int& newHeight, const unsigned int& newTileTypes);
        bool tryPause();
        bool tryUnpause();
        void updateElapsedTime();
        

        int                 selectedIndex1;
        int                 selectedIndex2;
        unsigned int        matchesMade;
        uint32_t            milliStartTime;
        uint32_t            milliElapsedTime;
        uint32_t            milliPointDelta;      

        MainWindowData          windowData;
        GameBoard               gameBoard;
        ScoreTable              scoreTable;
        MainWindowInterface*    mainWindow;
};
