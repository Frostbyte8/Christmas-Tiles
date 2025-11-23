#pragma once
#include "stdint_compat.h"

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