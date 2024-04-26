#pragma once

#include "../macros.hpp"
#include "UserStats.hpp"

SERDE_STRUCT(BeatSaver::Models, UserDetail,
    GETTER_FIELD(int, Id, "id");
    GETTER_FIELD(std::string, Username, "name");
    GETTER_FIELD_OPTIONAL(std::string, Hash, "hash");
    GETTER_FIELD(std::string, AvatarURL, "avatar");
    GETTER_FIELD_OPTIONAL(bool, Testplay, "testplay");
    GETTER_FIELD_OPTIONAL(UserStats, Stats, "stats");

    public:
        void GetAvatarImageAsync(std::function<void(std::optional<std::vector<uint8_t>>)>) const;
        std::optional<std::vector<uint8_t>> GetAvatarImage() const;
)
