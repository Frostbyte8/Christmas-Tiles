#pragma once

#include "../stdint_compat.h"
#include <time.h>
#include <vector>

namespace ScoreTableConstants {
    static const int MAX_NAME_LENGTH = 25;
    static const size_t MAX_SCORES = 10;
}

struct ScoreT {
    unsigned int score;
    unsigned int year;
    unsigned int month;
    unsigned int day;
    wchar_t* name;
};

class ScoreTable {
    
    public:
        ScoreTable();
        ~ScoreTable();

        // Accessors

        __forceinline const std::vector<ScoreT>& getScores() const { return scores; }

        // Public Functions

        bool insertScore(ScoreT& newScore, size_t index = -1);
        size_t isNewHighscore(const uint32_t& score) const;

    private:

        // Private Functions
        __forceinline bool tryInsertAndPop(ScoreT& newScore, size_t& index);

        std::vector<ScoreT> scores;
};