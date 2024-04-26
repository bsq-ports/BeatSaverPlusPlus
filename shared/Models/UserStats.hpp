#pragma once

#include "../macros.hpp"
#include "./UserDiffStats.hpp"

SERDE_STRUCT(BeatSaver::Models, UserStats,
    GETTER_FIELD(int, TotalUpvotes, "totalUpvotes");
    GETTER_FIELD(int, TotalDownvotes, "totalDownvotes");
    GETTER_FIELD(int, TotalMaps, "totalMaps");
    GETTER_FIELD(int, RankedMaps, "rankedMaps");
    GETTER_FIELD(float, AvgBpm, "avgBpm");
    GETTER_FIELD(float, AvgScore, "avgScore");
    GETTER_FIELD(float, AvgDuration, "avgDuration");
    GETTER_FIELD_OPTIONAL(std::string, FirstUpload, "firstUpload");
    GETTER_FIELD_OPTIONAL(std::string, LastUpload, "lastUpload");
    GETTER_FIELD(UserDiffStats, DiffStats, "diffStats");
)
