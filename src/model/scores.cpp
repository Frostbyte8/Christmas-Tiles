#include "scores.h"

ScoreTable::ScoreTable() {
    ScoreT tempScore;

    scores.reserve(ScoreTableConstants::MAX_SCORES);

    for(size_t i = 0; i < ScoreTableConstants::MAX_SCORES; ++i) {

        wchar_t* defaultName = (wchar_t*)malloc(sizeof(wchar_t) * (ScoreTableConstants::MAX_NAME_LENGTH + 1));
        wcscpy_s(defaultName, ScoreTableConstants::MAX_NAME_LENGTH, L"Player");
        defaultName[ScoreTableConstants::MAX_NAME_LENGTH] = 0;
        
        tempScore.name = defaultName;

        scores.push_back(tempScore);
    }

}

ScoreTable::~ScoreTable() {
    
    for(size_t i = 0; i < scores.size(); ++i) {
        if(scores[i].name != NULL) {
            free(scores[i].name);
            scores[i].name = NULL;
        }
    }

    scores.resize(0);
}

bool ScoreTable::tryAddScore(wchar_t* name, const uint32_t& score, const uint16_t& year, const uint8_t& month, const uint8_t& day) {

    ScoreT newScore;

    newScore.name = name;
    newScore.score = score;
    newScore.month = month;
    newScore.day = day;
    newScore.year = year;

    for(size_t i = 0; i < scores.size(); ++i) {
        if(newScore.score > scores[i].score) {

            scores.pop_back();
            scores.insert(scores.begin()+i, newScore);

            return true;

        }
    }

    return false;
}