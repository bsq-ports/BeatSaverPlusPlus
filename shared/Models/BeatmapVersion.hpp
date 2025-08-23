#pragma once

#include "../macros.hpp"
#include "./BeatmapDifficulty.hpp"
#include <future>

namespace BeatSaver::Models {
    struct Beatmap;
}

SERDE_STRUCT(BeatSaver::Models, BeatmapVersion,
    BEATSAVER_PLUSPLUS_GETTER_FIELD(std::string, CreatedAt, "createdAt");
    BEATSAVER_PLUSPLUS_GETTER_FIELD_OPTIONAL(int, SageScore, "sageScore");
    BEATSAVER_PLUSPLUS_GETTER_FIELD(std::vector<BeatmapDifficulty>, Diffs, "diffs");
    BEATSAVER_PLUSPLUS_GETTER_FIELD_OPTIONAL(std::string, Feedback, "feedback");
    BEATSAVER_PLUSPLUS_GETTER_FIELD(std::string, Hash, "hash");
    BEATSAVER_PLUSPLUS_GETTER_FIELD_OPTIONAL(std::string, Key, "key");
    BEATSAVER_PLUSPLUS_GETTER_FIELD(std::string, State, "state"); // Enum with values Uploaded, Testplay, Published, Feedback
    BEATSAVER_PLUSPLUS_GETTER_FIELD(std::string, DownloadURL, "downloadURL");
    BEATSAVER_PLUSPLUS_GETTER_FIELD(std::string, CoverURL, "coverURL");
    BEATSAVER_PLUSPLUS_GETTER_FIELD(std::string, PreviewURL, "previewURL");

    public:
        BEATSAVER_PLUSPLUS_EXPORT std::optional<std::filesystem::path> DownloadBeatmap(Beatmap const& beatmap, std::function<void(float)> progressReport) const;
        BEATSAVER_PLUSPLUS_EXPORT std::future<std::optional<std::filesystem::path>> DownloadBeatmapAsync(Beatmap const& beatmap, std::function<void(float)> progressReport) const;
        BEATSAVER_PLUSPLUS_EXPORT void DownloadBeatmapAsync(Beatmap const& beatmap, std::function<void(std::optional<std::filesystem::path>)> onFinished, std::function<void(float)> progressReport) const;

        BEATSAVER_PLUSPLUS_EXPORT std::optional<std::vector<uint8_t>> GetCoverImage() const;
        BEATSAVER_PLUSPLUS_EXPORT void GetCoverImageAsync(std::function<void(std::optional<std::vector<uint8_t>>)> onFinished) const;
        BEATSAVER_PLUSPLUS_EXPORT std::future<std::optional<std::vector<uint8_t>>> GetCoverImageAsync() const;

        BEATSAVER_PLUSPLUS_EXPORT std::optional<std::vector<uint8_t>> GetPreview() const;
        BEATSAVER_PLUSPLUS_EXPORT void GetPreviewAsync(std::function<void(std::optional<std::vector<uint8_t>>)> onFinished) const;
        BEATSAVER_PLUSPLUS_EXPORT std::future<std::optional<std::vector<uint8_t>>> GetPreviewAsync() const;
);
