#pragma once

#include "../macros.hpp"

SERDE_STRUCT(BeatSaver::Models, Stats,
    GETTER_FIELD_DEFAULT(int, Plays, -1, "plays");
    GETTER_FIELD_DEFAULT(int, Downloads, -1, "downloads");
    GETTER_FIELD(int, UpVotes, "upvotes");
    GETTER_FIELD(int, DownVotes, "downvotes");
    GETTER_FIELD(float, Score, "score");
);
