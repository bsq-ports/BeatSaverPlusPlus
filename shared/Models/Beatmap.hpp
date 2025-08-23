#pragma once

#include "../macros.hpp"
#include "./UserDetail.hpp"
#include "./Stats.hpp"
#include "./BeatmapVersion.hpp"
#include "./Metadata.hpp"

SERDE_STRUCT(BeatSaver::Models, Beatmap,
    BEATSAVER_PLUSPLUS_GETTER_FIELD(std::string, Id, "id");
    BEATSAVER_PLUSPLUS_GETTER_FIELD(std::string, Name, "name");
    BEATSAVER_PLUSPLUS_GETTER_FIELD(std::string, Description, "description");
    BEATSAVER_PLUSPLUS_GETTER_FIELD(UserDetail, Uploader, "uploader");
    BEATSAVER_PLUSPLUS_GETTER_FIELD(struct Metadata, Metadata, "metadata");
    BEATSAVER_PLUSPLUS_GETTER_FIELD(struct Stats, Stats, "stats");
    BEATSAVER_PLUSPLUS_GETTER_FIELD(std::string, Uploaded, "uploaded");
    BEATSAVER_PLUSPLUS_GETTER_FIELD(bool, Automapper, "automapper");
    BEATSAVER_PLUSPLUS_GETTER_FIELD(bool, Ranked, "ranked");
    BEATSAVER_PLUSPLUS_GETTER_FIELD(bool, Qualified, "qualified");
    BEATSAVER_PLUSPLUS_GETTER_FIELD(std::vector<BeatmapVersion>, Versions, "versions");

    public:
        BEATSAVER_PLUSPLUS_EXPORT std::string CreateFolderName(const BeatmapVersion& version) const { return fmt::format("{} ({} - {})", version.Key.value_or(Id), Metadata.SongName, Metadata.LevelAuthorName); }
        std::string CreateFolderName() const { return CreateFolderName(Versions.front()); }

        BEATSAVER_PLUSPLUS_EXPORT std::optional<std::filesystem::path> DownloadLatestBeatmap(Beatmap const& beatmap, std::function<void(float)> progressReport) const {
            return Versions.front().DownloadBeatmap(*this, progressReport);
        }

        BEATSAVER_PLUSPLUS_EXPORT std::future<std::optional<std::filesystem::path>> DownloadLatestBeatmapAsync(Beatmap const& beatmap, std::function<void(float)> progressReport) const {
            return Versions.front().DownloadBeatmapAsync(*this, progressReport);
        }

        BEATSAVER_PLUSPLUS_EXPORT void DownloadLatestBeatmapAsync(Beatmap const& beatmap, std::function<void(std::optional<std::filesystem::path>)> onFinished, std::function<void(float)> progressReport) const {
            return Versions.front().DownloadBeatmapAsync(*this, onFinished, progressReport);
        }

        BEATSAVER_PLUSPLUS_EXPORT std::optional<std::vector<uint8_t>> GetLatestCoverImage() const {
            return Versions.front().GetCoverImage();
        }

        BEATSAVER_PLUSPLUS_EXPORT void GetLatestCoverImageAsync(std::function<void(std::optional<std::vector<uint8_t>>)> onFinished) const {
            return Versions.front().GetCoverImageAsync(onFinished);
        }

        BEATSAVER_PLUSPLUS_EXPORT std::future<std::optional<std::vector<uint8_t>>> GetLatestCoverImageAsync() const {
            return Versions.front().GetCoverImageAsync();
        }

        BEATSAVER_PLUSPLUS_EXPORT std::optional<std::vector<uint8_t>> GetLatestPreview() const {
            return Versions.front().GetPreview();
        }

        BEATSAVER_PLUSPLUS_EXPORT void GetLatestPreviewAsync(std::function<void(std::optional<std::vector<uint8_t>>)> onFinished) const {
            return Versions.front().GetPreviewAsync(onFinished);
        }

        BEATSAVER_PLUSPLUS_EXPORT std::future<std::optional<std::vector<uint8_t>>> GetLatestPreviewAsync() const {
            return Versions.front().GetPreviewAsync();
        }
);
