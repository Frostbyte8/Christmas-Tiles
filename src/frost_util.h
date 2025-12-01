#pragma once
#include "stdint_compat.h"

#ifdef _DEBUG
#define NODEFAULT assert(0)
#else
#define NODEFAULT __assume(0)
#endif // DEBUG

namespace FrostUtil {

    // At some point, I'll template this
    __forceinline uint8_t ClampInts(const uint8_t& newValue, const uint8_t& minValue, const uint8_t& maxValue) {
        if(newValue > maxValue) {
            return maxValue;
        }
        else if(newValue < minValue) {
            return minValue;
        }

        return newValue;
    }
}