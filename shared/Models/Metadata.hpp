#pragma once

#include "../macros.hpp"

SERDE_STRUCT(BeatSaver::Models, Metadata,
    GETTER_FIELD(float, BPM, "bpm");
    GETTER_FIELD(int, Duration, "duration");
    GETTER_FIELD(std::string, SongName, "songName");
    GETTER_FIELD(std::string, SongSubName, "songSubName");
    GETTER_FIELD(std::string, SongAuthorName, "songAuthorName");
    GETTER_FIELD(std::string, LevelAuthorName, "levelAuthorName");
);
