#pragma once

#include "../macros.hpp"

SERDE_STRUCT(BeatSaver::Models, UserDiffStats,
    GETTER_FIELD(int, Total, "total");
    GETTER_FIELD(int, Easy, "easy");
    GETTER_FIELD(int, Normal, "normal");
    GETTER_FIELD(int, Hard, "hard");
    GETTER_FIELD(int, Expert, "expert");
    GETTER_FIELD(int, ExpertPlus, "expertPlus");
)
