#pragma once

#include "../macros.hpp"
#include "./BeatmapParitySummary.hpp"

SERDE_STRUCT(BeatSaver::Models, BeatmapDifficulty,
    GETTER_FIELD(float, NJS, "njs");
    GETTER_FIELD(float, Offset, "offset");
    GETTER_FIELD(int, Notes, "notes");
    GETTER_FIELD(int, Bombs, "bombs");
    GETTER_FIELD(int, Obstacles, "obstacles");
    GETTER_FIELD(double, NPS, "nps");
    GETTER_FIELD(double, Length, "length");
    GETTER_FIELD(std::string, Characteristic, "characteristic");
    GETTER_FIELD(std::string, Difficulty, "difficulty");
    GETTER_FIELD(int, Events, "events");
    GETTER_FIELD(bool, Chroma, "chroma");
    GETTER_FIELD(bool, ME, "me");
    GETTER_FIELD(bool, NE, "ne");
    GETTER_FIELD(bool, Cinema, "cinema");
    GETTER_FIELD(double, Seconds, "seconds");
    GETTER_FIELD(BeatmapParitySummary, ParitySummary, "paritySummary");
    GETTER_FIELD_OPTIONAL(float, Stars, "stars");
);
