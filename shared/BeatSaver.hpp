#pragma once

#include "./_config.h"
#include "./macros.hpp"
#include "./Models/Page.hpp"
#include "./Models/UserDetail.hpp"

#include "web-utils/shared/DownloaderUtility.hpp"

#include <chrono>
#include <future>
#include <map>
#include <type_traits>

#if defined(BEATSAVER_PLUSPLUS_AUTO_INIT) && __has_include("songcore/shared/SongCore.hpp")
#include "songcore/shared/SongCore.hpp"
/// @brief automatically initialize the library if you define the macro and have songcore as a dependency as well
static __attribute__((constructor)) void beatsaver_plusplus_auto_init() {
    BeatSaver::API::Init(SongCore::API::Loading::GetPreferredCustomLevelPath());
}
#endif

/// API in this header is based on https://api.beatsaver.com/docs/index.html?url=./swagger.json

#define BEATSAVER_API_URL "https://api.beatsaver.com"
#define BEATSAVER_CDN_URL "https://cdn.beatsaver.com"

namespace BeatSaver::API {
    /// @brief initialize with the root dir where songs should be output
    BEATSAVER_PLUSPLUS_EXPORT void Init(std::filesystem::path defaultOutputRootPath);

    /// @brief Get the root dir where songs are saved to. defaults to the SongCore/CustomLevels directory if it was never initialized
    BEATSAVER_PLUSPLUS_EXPORT std::filesystem::path GetDefaultOutputPath();

    /// @brief downloads a song zip from the url to a given path
    /// @param url the url to download the file from
    /// @param outputPath the output directory, should not be CustomLevels, but the output path
    /// @return bool future success, if download failed or extraction of the file failed, false will return.
    std::future<bool> BEATSAVER_PLUSPLUS_EXPORT DownloadSongZipAsync(std::string url, std::filesystem::path outputPath, std::function<void(float)> progressReport = nullptr);

    /// @brief downloads a song zip from the url to a given path
    /// @param url the url to download the file from
    /// @param outputPath the output directory, should not be CustomLevels, but the output path
    /// @return bool success, if download failed or extraction of the file failed, false will return.
    bool BEATSAVER_PLUSPLUS_EXPORT DownloadSongZip(std::string url, std::filesystem::path outputPath, std::function<void(float)> progressReport = nullptr);

    DECLARE_SIMPLE_RESPONSE_T(Models, Page);
    DECLARE_SIMPLE_RESPONSE_T(Models, Beatmap);
    DECLARE_SIMPLE_RESPONSE_T(Models, UserDetail);

    enum class Filter {
        Ignore, // ignore this filter
        Include, // include maps with this
        Exclude, // exclude maps with this
    };

    using timestamp = std::variant<std::string, std::chrono::time_point<std::chrono::system_clock>>;

    /// @brief future for an optional T
    /// @brief function called with an optional T as single arg
    template<typename T>
    using finished_opt_function = std::function<void(std::optional<T>)>;

    namespace Maps {
        enum class LatestSortOrder {
            FirstPublished,
            Updated,
            LastPublished,
            Created,
            Curated
        };

        struct LatestQueryOptions {
            /// @brief order by which to sort the latest maps
            std::optional<LatestSortOrder> sortOrder = std::nullopt;
            /// @brief page size for the search
            std::optional<int> pageSize = std::nullopt;
            /// @brief whether to include verified mappers
            Filter verified = Filter::Ignore;
            /// @brief whether to include auto mapped levels
            Filter automapper = Filter::Exclude;

            /// @brief only return maps before the given time
            std::optional<timestamp> before = std::nullopt;
            /// @brief only return maps after the given time
            std::optional<timestamp> after = std::nullopt;

            WebUtils::URLOptions::QueryMap GetQueries() const;
        };

        struct CollaborationQueryOptions {
            /// @brief only maps made before this timestamp will be returned
            std::optional<timestamp> before = std::nullopt;
            /// @brief size of page returned
            std::optional<int> pageSize = std::nullopt;

            WebUtils::URLOptions::QueryMap GetQueries() const;
        };

        /// @brief get a beatmap by its key (id)
        BEATSAVER_PLUSPLUS_EXPORT void GetBeatmapByKeyAsync(std::string key, finished_opt_function<Models::Beatmap> onFinished);

        /// @brief get multiple beatmaps by their keys (ids)
        BEATSAVER_PLUSPLUS_EXPORT void GetBeatmapsByKeysAsync(std::span<std::string const> keys, finished_opt_function<std::unordered_map<std::string, Models::Beatmap>> onFinished);

        /// @brief get a beatmap by its hash
        BEATSAVER_PLUSPLUS_EXPORT void GetBeatmapByHashAsync(std::string hash, finished_opt_function<Models::Beatmap> onFinished);

        /// @brief get a page of beatmaps by a user
        BEATSAVER_PLUSPLUS_EXPORT void GetBeatmapsByUserAsync(int id, int page, finished_opt_function<Models::Page> onFinished);

        /// @brief get a page of collaboration beatmaps by a user
        BEATSAVER_PLUSPLUS_EXPORT void GetCollaborationsByUserAsync(int id, finished_opt_function<Models::Page> onFinished, CollaborationQueryOptions collaborationOptions = {});

        /// @brief get a page of the latest maps
        BEATSAVER_PLUSPLUS_EXPORT void GetLatestAsync(finished_opt_function<Models::Page> onFinished, LatestQueryOptions queryOptions = {});

        /// @brief get a beatmap by its key (id)
        BEATSAVER_PLUSPLUS_EXPORT std::optional<Models::Beatmap> GetBeatmapByKey(std::string key);

        /// @brief get multiple beatmaps by their keys (ids)
        BEATSAVER_PLUSPLUS_EXPORT std::optional<std::unordered_map<std::string, Models::Beatmap>> GetBeatmapsByKeys(std::span<std::string const> keys);

        /// @brief get a beatmap by its hash
        BEATSAVER_PLUSPLUS_EXPORT std::optional<Models::Beatmap> GetBeatmapByHash(std::string hash);

        /// @brief get a page of beatmaps by a user
        BEATSAVER_PLUSPLUS_EXPORT std::optional<Models::Page> GetBeatmapsByUser(int id, int page);

        /// @brief get a page of collaboration beatmaps by a user
        BEATSAVER_PLUSPLUS_EXPORT std::optional<Models::Page> GetCollaborationsByUser(int id, CollaborationQueryOptions = {});

        /// @brief get a page of the latest maps
        BEATSAVER_PLUSPLUS_EXPORT std::optional<Models::Page> GetLatest(LatestQueryOptions queryOptions = {});
    }

    namespace Users {
        /// @brief get a users details by their id
        BEATSAVER_PLUSPLUS_EXPORT void GetUserByIdAsync(int id, finished_opt_function<Models::UserDetail> onFinished);

        /// @brief get multiple users details by their id
        BEATSAVER_PLUSPLUS_EXPORT void GetUsersByIdsAsync(std::span<int const> id, finished_opt_function<std::unordered_map<int, Models::UserDetail>> onFinished);

        /// @brief get a users details by their name
        BEATSAVER_PLUSPLUS_EXPORT void GetUserByNameAsync(std::string userName, finished_opt_function<Models::UserDetail> onFinished);

        /// @brief get a users details by their id
        BEATSAVER_PLUSPLUS_EXPORT std::optional<Models::UserDetail> GetUserById(int id);

        /// @brief get multiple users details by their id
        BEATSAVER_PLUSPLUS_EXPORT std::optional<std::unordered_map<int, Models::UserDetail>> GetUsersByIds(std::span<int const> ids);

        /// @brief get a users details by their name
        BEATSAVER_PLUSPLUS_EXPORT std::optional<Models::UserDetail> GetUserByName(std::string userName);
    }

    namespace Search {
        enum class SearchSortOrder {
            Latest,
            Relevance,
            Rating,
            Curated
        };

        struct SearchQueryOptions {
            /// @brief query to filter on
            std::optional<std::string> query = std::nullopt;
            /// @brief page index of the results
            std::optional<int> pageIndex = std::nullopt;
            /// @brief order by which to sort
            SearchSortOrder sortOrder = SearchSortOrder::Latest;
            /// @brief whether to include automapper in the results
            Filter automapper = Filter::Exclude;
            /// @brief whether to include chroma in the results
            Filter chroma = Filter::Ignore;
            /// @brief whether to include noodle in the results
            Filter noodle = Filter::Ignore;
            /// @brief whether to include mapping ext in the results
            Filter me = Filter::Ignore;
            /// @brief whether to include cinema in the results
            Filter cinema = Filter::Ignore;
            /// @brief whether to include ranked in the results
            Filter ranked = Filter::Ignore;
            /// @brief whether to include verified in the results
            Filter verified = Filter::Ignore;
            /// @brief whether to include fullspread in the results
            Filter fullspread = Filter::Ignore;
            /// @brief start of time filter
            std::optional<timestamp> from = std::nullopt;
            /// @brief end of time filter
            std::optional<timestamp> to = std::nullopt;
            /// @brief tags to include in the search
            std::vector<std::string> includeTags = {};
            /// @brief tags to exclude in the search
            std::vector<std::string> excludeTags = {};
            /// @brief max bpm allowed
            std::optional<float> maxBpm = std::nullopt;
            /// @brief min bpm allowed
            std::optional<float> minBpm = std::nullopt;
            /// @brief max length allowed
            std::optional<int> maxDuration = std::nullopt;
            /// @brief min length allowed
            std::optional<int> minDuration = std::nullopt;
            /// @brief max nps allowed
            std::optional<float> maxNps = std::nullopt;
            /// @brief min nps allowed
            std::optional<float> minNps = std::nullopt;
            /// @brief max rating allowed
            std::optional<float> maxRating = std::nullopt;
            /// @brief min rating allowed
            std::optional<float> minRating = std::nullopt;

            WebUtils::URLOptions::QueryMap GetQueries() const;
        };

        /// @brief search for beatmaps with pagination
        BEATSAVER_PLUSPLUS_EXPORT void GetPageAsync(int page, finished_opt_function<Models::Page> onFinished, SearchQueryOptions queryOptions = {});

        /// @brief search for beatmaps with pagination
        BEATSAVER_PLUSPLUS_EXPORT std::optional<Models::Page> GetPage(int page, SearchQueryOptions queryOptions = {});

        /// @brief provides a readonly span of some map feel tags that are available on beatsaver. these tags were pulled at a moment in time and are not guaranteed to still exist
        BEATSAVER_PLUSPLUS_EXPORT std::span<const std::string> GetMapFeelTags();

        /// @brief provides a readonly span of some genre tags that are available on beatsaver. these tags were pulled at a moment in time and are not guaranteed to still exist
        BEATSAVER_PLUSPLUS_EXPORT std::span<const std::string> GetGenreTags();
    }

    namespace Download {

        /// @brief helper struct containing the information for a map download
        struct BEATSAVER_PLUSPLUS_EXPORT BeatmapDownloadInfo {
            BeatmapDownloadInfo() = default;
            /// @brief info from direct values
            BeatmapDownloadInfo(std::string Key, std::string DownloadURL, std::string FolderName) : Key(Key), DownloadURL(DownloadURL), FolderName(FolderName) {}
            /// @brief info from a beatmap, gets the front version to download
            BeatmapDownloadInfo(Models::Beatmap const& beatmap) : BeatmapDownloadInfo(beatmap, beatmap.Versions.front()) {}
            /// @brief info from a beatmap and version
            BeatmapDownloadInfo(Models::Beatmap const& beatmap, Models::BeatmapVersion const& version);

            std::string const Key;
            std::string const DownloadURL;
            std::string const FolderName;
        };

        /// @brief download a beatmap from the provided beatmapversion
        BEATSAVER_PLUSPLUS_EXPORT void DownloadBeatmapAsync(BeatmapDownloadInfo info, finished_opt_function<std::filesystem::path> onFinished, std::function<void(float)> progressReport = nullptr);

        /// @brief download multiple beatmaps asynchronously, ratelimited to ~4 async requests.
        /// @return map of the downloaded beatmaps' keys, if a download failed its corresponding path wont be set
        BEATSAVER_PLUSPLUS_EXPORT std::future<std::unordered_map<std::string, std::optional<std::filesystem::path>>> DownloadBeatmapsAsync(std::span<BeatmapDownloadInfo const> infos, std::function<void(int, int)> progressReport = nullptr);

        /// @brief download multiple beatmaps asynchronously, ratelimited to ~4 async requests.
        BEATSAVER_PLUSPLUS_EXPORT void DownloadBeatmapsAsync(std::span<BeatmapDownloadInfo const> infos, finished_opt_function<std::unordered_map<std::string, std::optional<std::filesystem::path>>> onFinished, std::function<void(int, int)> progressReport = nullptr);

        /// @brief get the preview sound from the provided beatmapversion
        BEATSAVER_PLUSPLUS_EXPORT void GetPreviewAsync(Models::BeatmapVersion const& beatmap, finished_opt_function<std::vector<uint8_t>> onFinished);

        /// @brief get the cover image data from the provided beatmapversion
        BEATSAVER_PLUSPLUS_EXPORT void GetCoverImageAsync(Models::BeatmapVersion const& beatmap, finished_opt_function<std::vector<uint8_t>> onFinished);

        /// @brief download a beatmap from the provided download info
        /// @return output path where it was written to
        BEATSAVER_PLUSPLUS_EXPORT std::optional<std::filesystem::path> DownloadBeatmap(BeatmapDownloadInfo downloadInfo, std::function<void(float)> progressReport = nullptr);

        /// @brief download multiple beatmaps, ratelimited to ~4 async requests.
        /// @return readonly span of the infos to use to download these maps
        BEATSAVER_PLUSPLUS_EXPORT std::unordered_map<std::string, std::optional<std::filesystem::path>> DownloadBeatmaps(std::span<BeatmapDownloadInfo const> infos, std::function<void(int, int)> progressReport = nullptr);

        /// @brief get the preview sound from the provided beatmapversion
        BEATSAVER_PLUSPLUS_EXPORT std::optional<std::vector<uint8_t>> GetPreview(Models::BeatmapVersion const& beatmap);

        /// @brief get the cover image data from the provided beatmapversion
        BEATSAVER_PLUSPLUS_EXPORT std::optional<std::vector<uint8_t>> GetCoverImage(Models::BeatmapVersion const& beatmap);
    }
}
