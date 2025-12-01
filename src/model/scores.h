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
    ScoreT() : name(NULL), score(0), year(1995), month(12), day(25) {}
};

class ScoreTable {
    
    public:
        ScoreTable();
        ~ScoreTable();
        size_t isNewHighscore(const uint32_t& score) const;

        bool insertScore(ScoreT& newScore, size_t index = -1);

        __forceinline const std::vector<ScoreT>& getScores() const { return scores; }

    private:
        std::vector<ScoreT> scores;
};