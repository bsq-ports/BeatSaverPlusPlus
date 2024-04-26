#pragma once

#include "../macros.hpp"

SERDE_STRUCT(BeatSaver::Models, BeatmapParitySummary,
    GETTER_FIELD(int, Errors, "errors");
    GETTER_FIELD(int, Warns, "warns");
    GETTER_FIELD(int, Resets, "resets");
);
