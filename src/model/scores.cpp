#include "scores.h"
#include "../constants.h"

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

size_t ScoreTable::isNewHighscore(const uint32_t& score) {
    for(size_t i = 0; i < scores.size(); ++i) {
        if(score > scores[i].score) {
            return i;
        }
    }

    return 10;
}

void ScoreTable::insertScore(wchar_t* name, const uint32_t& score, const uint16_t& year, const uint8_t& month, const uint8_t& day, const size_t& index) {
    ScoreT newScore;

    newScore.name = name;
    newScore.score = score < ChristmasTilesConstants::MAX_SCORE ? score : ChristmasTilesConstants::MAX_SCORE;
    newScore.month = month < 12 ? month : 12;
    newScore.day = day < 31 ? day : 31;

    newScore.year = year > 9999 ? 9999 : (year < 1000 ? 1000 : year);

    scores.pop_back();
    scores.insert(scores.begin()+index, newScore);

}

bool ScoreTable::tryAddScore(wchar_t* name, const uint32_t& score, const uint16_t& year, const uint8_t& month, const uint8_t& day) {

    ScoreT newScore;

    newScore.score = score < ChristmasTilesConstants::MAX_SCORE ? score : ChristmasTilesConstants::MAX_SCORE;
    newScore.month = month < 12 ? month : 12;
    newScore.day = day < 31 ? day : 31;
    newScore.year = year > 9999 ? 9999 : (year < 1000 ? 1000 : year);

    for(size_t i = 0; i < scores.size(); ++i) {
        if(newScore.score > scores[i].score) {

            // TODO: Overwrite score rather than insert?

            newScore.name = (wchar_t*)malloc(sizeof(wchar_t) * (ScoreTableConstants::MAX_NAME_LENGTH + 1));
            wcscpy_s(newScore.name, ScoreTableConstants::MAX_NAME_LENGTH, name);

            free(scores[scores.size()-1].name);
            scores[scores.size()-1].name = NULL;
            scores.pop_back();
            scores.insert(scores.begin()+i, newScore);

            return true;

        }
    }

    return false;
}