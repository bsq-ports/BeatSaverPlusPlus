#pragma once

#include "../macros.hpp"
#include "./Beatmap.hpp"

SERDE_STRUCT(BeatSaver::Models, Page,
    GETTER_FIELD(std::vector<Beatmap>, Docs, "docs");
    GETTER_FIELD_OPTIONAL(UserDetail, User, "user");
);
