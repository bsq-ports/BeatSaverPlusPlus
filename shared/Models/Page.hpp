#pragma once

#include "../macros.hpp"
#include "./Beatmap.hpp"

SERDE_STRUCT(BeatSaver::Models, Page,
    BEATSAVER_PLUSPLUS_GETTER_FIELD(std::vector<Beatmap>, Docs, "docs");
    BEATSAVER_PLUSPLUS_GETTER_FIELD_OPTIONAL(UserDetail, User, "user");
);
