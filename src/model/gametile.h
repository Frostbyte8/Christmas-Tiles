#pragma once

#include "../stdint_compat.h"

namespace TileFlags {
    enum flags {
        UNFLIPPED   = 0x00,     // Not yet flipped
        FLIPPED     = 0x01,     // Flipped, but not matched
        MATCHED     = 0x02,     // Matched, and cannot be selected again.
    };
}

namespace TileTypes {
    enum TileTypes {
        UNFLIPPED   = 0,
        FREE        = 1,
    };
}

struct TileData {
    unsigned int ID;
    unsigned int flags;
};