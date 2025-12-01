#pragma once

#include "../stdint_compat.h"
#include <time.h>
#include <vector>

/*
    time_t now = time(NULL);
    tm *t = localtime(&now);

    printf("%d", t->tm_hour);
*/

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

        bool insertScore(ScoreT& newScore, size_t index);

        void insertScore(wchar_t* name, const uint32_t& score, const uint16_t& year, const uint8_t& month, const uint8_t& day, const size_t& index);
        bool tryAddScore(wchar_t* name, const uint32_t& score, const uint16_t& year, const uint8_t& month, const uint8_t& day);
        __forceinline const std::vector<ScoreT>& getScores() const { return scores; }

    private:
        std::vector<ScoreT> scores;
};