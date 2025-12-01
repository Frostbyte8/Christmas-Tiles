#include "scores.h"
#include "../constants.h"
#include "../frost_util.h"

ScoreTable::ScoreTable() {
    ScoreT tempScore;

    // We need one more so that we can pop the 11th score off during insertion,
    // this way we don't pop, insert, and cause an exception that leaves the
    // container in an invalid state.    
    scores.reserve(ScoreTableConstants::MAX_SCORES + 1);

    for(size_t i = 0; i < ScoreTableConstants::MAX_SCORES; ++i) {

        wchar_t* defaultName = (wchar_t*)malloc(sizeof(wchar_t) * (ScoreTableConstants::MAX_NAME_LENGTH + 1));
        wcscpy_s(defaultName, ScoreTableConstants::MAX_NAME_LENGTH, L"");
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

size_t ScoreTable::isNewHighscore(const uint32_t& score) const {
    for(size_t i = 0; i < scores.size(); ++i) {
        if(score > scores[i].score) {
            return i;
        }
    }

    return -1;
}

bool ScoreTable::insertScore(ScoreT& newScore, size_t index) {

    newScore.score = FrostUtil::ClampInts(newScore.score, static_cast<unsigned int>(0), ChristmasTilesConstants::MAX_SCORE);
    wchar_t* name = (wchar_t*)malloc(sizeof(wchar_t) * (ScoreTableConstants::MAX_NAME_LENGTH + 1));
    
    // TODO: Make sure date is a valid date and remove that code from readScores();

    if(!name) {
        return false;
    }

    if(newScore.name) {
        wcscpy_s(name, ScoreTableConstants::MAX_NAME_LENGTH, newScore.name);
    }
    else {
        wcscpy_s(name, ScoreTableConstants::MAX_NAME_LENGTH, L"Player");
    }

    newScore.name = name;
        
    if(index != -1) {

        try { 
            scores.insert(scores.begin()+index, newScore);
        }
        catch(std::exception) {
            free(newScore.name);
            return false;
        }
        free(scores[ScoreTableConstants::MAX_SCORES].name);
        scores[ScoreTableConstants::MAX_SCORES].name = NULL;
        scores.pop_back();
        return true;
    }

    for(index = 0; index < scores.size(); ++index) {
        if(newScore.score > scores[index].score) {

            try { 
                scores.insert(scores.begin()+index, newScore);
            }
            catch(std::exception) {
                free(newScore.name);
                return false;
            }

            free(scores[ScoreTableConstants::MAX_SCORES].name);
            scores[ScoreTableConstants::MAX_SCORES].name = NULL;
            scores.pop_back();

            return true;
        }
    }

    free(newScore.name);
    newScore.name = NULL;
    return false;
}