#pragma once

#include "../macros.hpp"
#include "./UserDetail.hpp"
#include "./Stats.hpp"
#include "./BeatmapVersion.hpp"
#include "./Metadata.hpp"

SERDE_STRUCT(BeatSaver::Models, Beatmap,
    GETTER_FIELD(std::string, Id, "id");
    GETTER_FIELD(std::string, Name, "name");
    GETTER_FIELD(std::string, Description, "description");
    GETTER_FIELD(UserDetail, Uploader, "uploader");
    GETTER_FIELD(struct Metadata, Metadata, "metadata");
    GETTER_FIELD(struct Stats, Stats, "stats");
    GETTER_FIELD(std::string, Uploaded, "uploaded");
    GETTER_FIELD(bool, Automapper, "automapper");
    GETTER_FIELD(bool, Ranked, "ranked");
    GETTER_FIELD(bool, Qualified, "qualified");
    GETTER_FIELD(std::vector<BeatmapVersion>, Versions, "versions");

    public:
        std::optional<std::filesystem::path> DownloadLatestBeatmap(Beatmap const& beatmap, std::function<void(float)> progressReport) const {
            return Versions.front().DownloadBeatmap(*this, progressReport);
        }

        std::future<std::optional<std::filesystem::path>> DownloadLatestBeatmapAsync(Beatmap const& beatmap, std::function<void(float)> progressReport) const {
            return Versions.front().DownloadBeatmapAsync(*this, progressReport);
        }

        void DownloadLatestBeatmapAsync(Beatmap const& beatmap, std::function<void(std::optional<std::filesystem::path>)> onFinished, std::function<void(float)> progressReport) const {
            return Versions.front().DownloadBeatmapAsync(*this, onFinished, progressReport);
        }

        std::optional<std::vector<uint8_t>> GetLatestCoverImage() const {
            return Versions.front().GetCoverImage();
        }

        void GetLatestCoverImageAsync(std::function<void(std::optional<std::vector<uint8_t>>)> onFinished) const {
            return Versions.front().GetCoverImageAsync(onFinished);
        }

        std::future<std::optional<std::vector<uint8_t>>> GetLatestCoverImageAsync() const {
            return Versions.front().GetCoverImageAsync();
        }

        std::optional<std::vector<uint8_t>> GetLatestPreview() const {
            return Versions.front().GetPreview();
        }

        void GetLatestPreviewAsync(std::function<void(std::optional<std::vector<uint8_t>>)> onFinished) const {
            return Versions.front().GetPreviewAsync(onFinished);
        }

        std::future<std::optional<std::vector<uint8_t>>> GetLatestPreviewAsync() const {
            return Versions.front().GetPreviewAsync();
        }
);
