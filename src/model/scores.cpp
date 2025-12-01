#include "scores.h"
#include "../constants.h"
#include "../frost_util.h"

//==============================================================================
// Constructors
//==============================================================================

ScoreTable::ScoreTable() {
    ScoreT tempScore = {0, 1995, 12, 25, NULL};

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

//==============================================================================
// Destructor
//==============================================================================

ScoreTable::~ScoreTable() {
    
    for(size_t i = 0; i < scores.size(); ++i) {
        if(scores[i].name != NULL) {
            free(scores[i].name);
            scores[i].name = NULL;
        }
    }

    scores.resize(0);
}

//==============================================================================
// Public Functions
//==============================================================================

//------------------------------------------------------------------------------
// insertScore - Inserts a score into the score table. The data passed in is
// sanatized.
// Note that the score structure passed in will be altered as needed.
//------------------------------------------------------------------------------

bool ScoreTable::insertScore(ScoreT& newScore, size_t index) {

    newScore.score = FrostUtil::ClampInts(newScore.score, static_cast<unsigned int>(0), ChristmasTilesConstants::MAX_SCORE);
    
    // Since we don't own the name, we need to make a copy of it.

    wchar_t* name = (wchar_t*)malloc(sizeof(wchar_t) * (ScoreTableConstants::MAX_NAME_LENGTH + 1));

    if(!name) {
        return false;
    }

    name[ScoreTableConstants::MAX_NAME_LENGTH] = 0;

    if(newScore.name) {
        wcscpy_s(name, ScoreTableConstants::MAX_NAME_LENGTH+1, newScore.name);
    }
    else {
        wcscpy_s(name, ScoreTableConstants::MAX_NAME_LENGTH+1, L"Player");
    }

    newScore.name = name;

    // Make sure the date is a valid date
    bool isDateValid = true;

    if(newScore.year > 8099) {
        // Years beyond 10000 aren't valid, not to mention, it might mean the score
        // was read as earlier than 1900.
        isDateValid = false; 
    }
    else if(newScore.month > 11) {
        isDateValid = false;
    }
    else if(newScore.day > 31) {
        isDateValid = false;
    }
    else if(newScore.day > 30) {
        // These months can only have 30 days.
        if(newScore.month == 1 || newScore.month == 3 || newScore.month == 5 || newScore.month == 8 || newScore.month == 10) {
            isDateValid = false;
        }
    }
    else if(newScore.day > 28 && newScore.month == 1) {
        // Leap year check
        if(newScore.year % 4 != 0) {
            isDateValid = false;
        }
    }

    if(!isDateValid) {
        newScore.year = 1995;
        newScore.month = 12;
        newScore.day = 25;
    }
    else {
        newScore.year += 1900;
        newScore.month += 1;
        // Day is already valid
    }
        
    if(index != -1) {
        return tryInsertAndPop(newScore, index);
    }

    for(index = 0; index < scores.size(); ++index) {
        if(newScore.score > scores[index].score) {
            return tryInsertAndPop(newScore, index);
        }
    }

    free(newScore.name);
    newScore.name = NULL;
    return false;
}

//------------------------------------------------------------------------------
// isNewHighscore - Checks if the score the player got qualifies as a new
// highscore.
//------------------------------------------------------------------------------

size_t ScoreTable::isNewHighscore(const uint32_t& score) const {
    for(size_t i = 0; i < scores.size(); ++i) {
        if(score > scores[i].score) {
            return i;
        }
    }

    return static_cast<size_t>(-1);
}

//==============================================================================
// Private Function
//==============================================================================

//------------------------------------------------------------------------------
// tryInsertAndPop - Tries to insert a score, and if it does, it then releases
// the last scores resources and pop's it off the back of the score table.
//------------------------------------------------------------------------------

__forceinline bool ScoreTable::tryInsertAndPop(ScoreT& newScore, size_t& index) {

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