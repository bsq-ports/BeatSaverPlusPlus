#include "BeatSaver.hpp"
#include "Utils.hpp"
#include "logging.hpp"

#include "web-utils/shared/RatelimitedDispatcher.hpp"
#include "Exceptions.hpp"
#include <chrono>
#include <ctime>
#include <future>
#include <variant>

namespace BeatSaver::API {
    static const WebUtils::DownloaderUtility downloader{.userAgent=MOD_ID "/" VERSION " (+https://github.com/RedBrumbler/BeatSaverPlusPlus)"};
    static std::filesystem::path _defaultOutputRoothPath = "/sdcard/ModData/com.beatgames.beatsaber/Mods/SongCore/CustomLevels";

    void Init(std::filesystem::path defaultOutputRootPath) {
        _defaultOutputRoothPath = defaultOutputRootPath;
    }

    std::filesystem::path GetDefaultOutputPath() {
        return _defaultOutputRoothPath;
    }

    std::future<bool> DownloadSongZipAsync(std::string url, std::filesystem::path outputPath) {
        return std::async(std::launch::any, &DownloadSongZip, std::forward<std::string>(url), std::forward<std::filesystem::path>(outputPath));
    }

    bool DownloadSongZip(std::string url, std::filesystem::path outputPath) {
        auto data = downloader.Get<WebUtils::DataResponse>({url});
        if (!data.IsSuccessful() || !data.DataParsedSuccessful()) return false;
        auto& zipData = data.responseData.value();

        return Utils::ExtractAll(zipData, outputPath);
    }

    std::string timestamp_string(std::chrono::time_point<std::chrono::system_clock> timepoint) {
        using namespace std::chrono;
        auto since_epoch = timepoint.time_since_epoch();
        auto years_since = duration_cast<years>(since_epoch);
        auto months_since = duration_cast<months>(since_epoch - years_since);
        auto days_since = duration_cast<days>(since_epoch - years_since - months_since);
        auto hours_since = duration_cast<hours>(since_epoch - years_since - months_since - days_since);
        auto minutes_since = duration_cast<minutes>(since_epoch - years_since - months_since - days_since - hours_since);
        auto seconds_since = duration_cast<seconds>(since_epoch - years_since - months_since - days_since - hours_since - minutes_since);
        return fmt::format("{}-{}-{}T{}:{}:{}Z", years_since.count() + 1900, months_since.count() + 1, days_since.count() + 1, hours_since.count(), minutes_since.count(), seconds_since.count());
    }

    std::string timestamp_string(timestamp const& stamp) {
        if (std::holds_alternative<std::string>(stamp)) return std::get<std::string>(stamp);
        else if (std::holds_alternative<std::chrono::time_point<std::chrono::system_clock>>(stamp)) return timestamp_string(std::get<std::chrono::time_point<std::chrono::system_clock>>(stamp));
        return "";
    }

    WebUtils::URLOptions::QueryMap Search::SearchQueryOptions::GetQueries() const {
        WebUtils::URLOptions::QueryMap queries;
        // from the docs: Options are a little weird, I may add another enum field in future to make this clearer. true = both, false = only ai, null = no ai
        switch (automapper) {
            break; case Filter::Ignore: queries.emplace("automapper", "true");
            break; case Filter::Include: queries.emplace("automapper", "false");
            break; case Filter::Exclude: break;
        }

        if (query.has_value()) queries["q"] = query.value();

        switch(sortOrder) {
            using enum SearchSortOrder;
            case Latest: { queries.emplace("sortOrder", "Latest"); } break;
            case Relevance: { queries.emplace("sortOrder", "Relevance"); } break;
            case Rating: { queries.emplace("sortOrder", "Rating"); } break;
            case Curated: { queries.emplace("sortOrder", "Curated"); } break;
        }

        if (chroma != Filter::Ignore) queries["chroma"] = std::to_string(chroma == Filter::Include);
        if (noodle != Filter::Ignore) queries["noodle"] = std::to_string(noodle == Filter::Include);
        if (me != Filter::Ignore) queries["me"] = std::to_string(me == Filter::Include);
        if (cinema != Filter::Ignore) queries["cinema"] = std::to_string(cinema == Filter::Include);
        if (ranked != Filter::Ignore) queries["ranked"] = std::to_string(ranked == Filter::Include);
        if (verified != Filter::Ignore) queries["verified"] = std::to_string(verified == Filter::Include);
        if (fullspread != Filter::Ignore) queries["fullspread"] = std::to_string(fullspread == Filter::Include);

        if (from.has_value()) queries["from"] = timestamp_string(from.value());
        if (to.has_value()) queries["to"] = timestamp_string(to.value());

        std::vector<std::string> tags;
        tags.reserve(includeTags.size() + excludeTags.size());
        for (auto t : includeTags) tags.emplace_back(t);
        for (auto t : excludeTags) tags.emplace_back(fmt::format("!{}", t));
        if (!tags.empty()) queries["tags"] = fmt::format("{}", fmt::join(tags, ","));

        if (maxBpm.has_value()) queries["maxBpm"] = std::to_string(maxBpm.value());
        if (minBpm.has_value()) queries["minBpm"] = std::to_string(minBpm.value());
        if (maxDuration.has_value()) queries["maxDuration"] = std::to_string(maxDuration.value());
        if (minDuration.has_value()) queries["minDuration"] = std::to_string(minDuration.value());
        if (maxNps.has_value()) queries["maxNps"] = std::to_string(maxNps.value());
        if (minNps.has_value()) queries["minNps"] = std::to_string(minNps.value());
        if (maxRating.has_value()) queries["maxRating"] = std::to_string(maxRating.value());
        if (minRating.has_value()) queries["minRating"] = std::to_string(minRating.value());

        return queries;
    }

    WebUtils::URLOptions::QueryMap Maps::LatestQueryOptions::GetQueries() const {
        WebUtils::URLOptions::QueryMap queries;
        // from the docs: Options are a little weird, I may add another enum field in future to make this clearer. true = both, false = only ai, null = no ai
        switch (automapper) {
            break; case Filter::Ignore: queries.emplace("automapper", "true");
            break; case Filter::Include: queries.emplace("automapper", "false");
            break; case Filter::Exclude: break;
        }

        if (sortOrder.has_value()) {
            switch (sortOrder.value()) {
                using enum LatestSortOrder;
                case FirstPublished: { queries.emplace("sortOrder", "FIRST_PUBLISHED"); } break;
                case Updated: { queries.emplace("sortOrder", "UPDATED"); } break;
                case LastPublished: { queries.emplace("sortOrder", "LAST_PUBLISHED"); } break;
                case Created: { queries.emplace("sortOrder", "CREATED"); } break;
                case Curated: { queries.emplace("sortOrder", "CURATED"); } break;
            }
        }

        if (pageSize.has_value()) queries["pageSize"] = std::to_string(std::clamp(pageSize.value(), 1, 100));
        if (verified != Filter::Ignore) queries["verified"] = std::to_string(verified == Filter::Include);

        if (before.has_value()) queries["before"] = timestamp_string(before.value());
        if (after.has_value()) queries["after"] = timestamp_string(after.value());

        return queries;
    }

    WebUtils::URLOptions::QueryMap Maps::CollaborationQueryOptions::GetQueries() const {
        WebUtils::URLOptions::QueryMap queries;

        if (before.has_value()) queries["before"] = timestamp_string(before.value());
        if (pageSize.has_value()) queries["pageSize"] = std::to_string(std::clamp(pageSize.value(), 1, 100));

        return queries;
    }

    namespace Maps {
        void GetBeatmapByKeyAsync(std::string key, finished_opt_function<Models::Beatmap> onFinished) {
            if (!onFinished) return;

            std::thread([](std::string key, finished_opt_function<Models::Beatmap> onFinished){
                onFinished(GetBeatmapByKey(std::forward<std::string>(key)));
            }, std::forward<std::string>(key), std::forward<finished_opt_function<Models::Beatmap>>(onFinished)).detach();
        }

        void GetBeatmapsByKeysAsync(std::span<std::string const> keys, finished_opt_function<std::unordered_map<std::string, Models::Beatmap>> onFinished) {
            if (!onFinished) return;

            std::thread([](std::vector<std::string> keys, finished_opt_function<std::unordered_map<std::string, Models::Beatmap>> onFinished){
                onFinished(GetBeatmapsByKeys(keys));
            },  std::vector<std::string>(keys.begin(), keys.end()), std::forward<finished_opt_function<std::unordered_map<std::string, Models::Beatmap>>>(onFinished)).detach();
        }

        void GetBeatmapByHashAsync(std::string hash, finished_opt_function<Models::Beatmap> onFinished) {
            if (!onFinished) return;

            std::thread([](std::string hash, finished_opt_function<Models::Beatmap> onFinished){
                onFinished(GetBeatmapByHash(hash));
            },  std::forward<std::string>(hash), std::forward<finished_opt_function<Models::Beatmap>>(onFinished)).detach();
        }

        void GetBeatmapsByUserAsync(int id, int page, finished_opt_function<Models::Page> onFinished) {
            if (!onFinished) return;

            std::thread([](int id, int page, finished_opt_function<Models::Page> onFinished){
                onFinished(GetBeatmapsByUser(id, page));
            },  id, page, std::forward<finished_opt_function<Models::Page>>(onFinished)).detach();
        }

        void GetCollaborationsByUserAsync(int id, finished_opt_function<Models::Page> onFinished, CollaborationQueryOptions queryOptions) {
            if (!onFinished) return;

            std::thread([](int id, finished_opt_function<Models::Page> onFinished, CollaborationQueryOptions queryOptions){
                onFinished(GetCollaborationsByUser(id, queryOptions));
            },  id, std::forward<finished_opt_function<Models::Page>>(onFinished), std::forward<CollaborationQueryOptions>(queryOptions)).detach();
        }

        void GetLatestAsync(finished_opt_function<Models::Page> onFinished, LatestQueryOptions queryOptions) {
            if (!onFinished) return;

            std::thread([](finished_opt_function<Models::Page> onFinished, LatestQueryOptions queryOptions){
                onFinished(GetLatest(queryOptions));
            }, std::forward<finished_opt_function<Models::Page>>(onFinished), std::forward<LatestQueryOptions>(queryOptions)).detach();
        }

        std::optional<Models::Beatmap> GetBeatmapByKey(std::string key) {
            WebUtils::URLOptions urlOptions {
                fmt::format(BEATSAVER_API_URL "/maps/id/{}", key)
            };
            try {
                auto response = downloader.Get<BeatmapResponse>(urlOptions);
                if (response.IsSuccessful() && response.DataParsedSuccessful()) {
                    return response.responseData;
                }

                DEBUG("Response was unsuccesful (httpCode: {}, curlStatus: {}, parseSuccesful: {})", response.HttpCode, response.CurlStatus, response.DataParsedSuccessful());
            } catch (JsonException const& e) {
                ERROR("Json exception thrown during deserialization: {}", e.what());
            }
            return std::nullopt;
        }

        std::optional<std::unordered_map<std::string, Models::Beatmap>> GetBeatmapsByKeys(std::span<std::string const> keys) {
            WebUtils::URLOptions urlOptions {
                fmt::format(BEATSAVER_API_URL "/maps/ids/{}", fmt::join(keys.subspan(0, std::min<std::size_t>(keys.size(), 50)), ","))
            };
            try {
                auto response = downloader.Get<WebUtils::JsonResponse>(urlOptions);
                if (response.IsSuccessful() && response.DataParsedSuccessful()) {
                    auto& json = response.responseData.value();
                    ERROR_CHECK(json);

                    std::unordered_map<std::string, Models::Beatmap> parsedResults;
                    auto memberEnd = json.MemberEnd();
                    for (auto itr = json.MemberBegin(); itr != memberEnd; itr++) {
                        parsedResults.emplace(
                            itr->name.Get<std::string>(),
                            itr->value.Get<Models::Beatmap>()
                        );
                    }
                    return parsedResults;
                }

                DEBUG("Response was unsuccesful (httpCode: {}, curlStatus: {}, parseSuccesful: {})", response.HttpCode, response.CurlStatus, response.DataParsedSuccessful());
            } catch (JsonException const& e) {
                ERROR("Json exception thrown during deserialization: {}", e.what());
            }
            return std::nullopt;
        }

        std::optional<Models::Beatmap> GetBeatmapByHash(std::string hash) {
            WebUtils::URLOptions urlOptions {
                fmt::format(BEATSAVER_API_URL "/maps/hash/{}", hash)
            };

            try {
                auto response = downloader.Get<BeatmapResponse>(urlOptions);
                if (response.IsSuccessful() && response.DataParsedSuccessful()) {
                    return response.responseData;
                }

                DEBUG("Response was unsuccesful (httpCode: {}, curlStatus: {}, parseSuccesful: {})", response.HttpCode, response.CurlStatus, response.DataParsedSuccessful());
            } catch (JsonException const& e) {
                ERROR("Json exception thrown during deserialization: {}", e.what());
            }

            return std::nullopt;
        }

        std::optional<Models::Page> GetBeatmapsByUser(int id, int page) {
            WebUtils::URLOptions urlOptions {
                fmt::format(BEATSAVER_API_URL "/maps/uploader/{}/{}", id, page)
            };

            try {
                auto response = downloader.Get<PageResponse>(urlOptions);
                if (response.IsSuccessful() && response.DataParsedSuccessful()) {
                    return response.responseData;
                }

                DEBUG("Response was unsuccesful (httpCode: {}, curlStatus: {}, parseSuccesful: {})", response.HttpCode, response.CurlStatus, response.DataParsedSuccessful());
            } catch (JsonException const& e) {
                ERROR("Json exception thrown during deserialization: {}", e.what());
            }

            return std::nullopt;
        }

        std::optional<Models::Page> GetCollaborationsByUser(int id, CollaborationQueryOptions queryOptions) {
            WebUtils::URLOptions urlOptions {
                fmt::format(BEATSAVER_API_URL "/maps/collaborations/{}", id),
                queryOptions.GetQueries()
            };

            try {
                auto response = downloader.Get<PageResponse>(urlOptions);
                if (response.IsSuccessful() && response.DataParsedSuccessful()) {
                    return response.responseData;
                }

                DEBUG("Response was unsuccesful (httpCode: {}, curlStatus: {}, parseSuccesful: {})", response.HttpCode, response.CurlStatus, response.DataParsedSuccessful());
            } catch (JsonException const& e) {
                ERROR("Json exception thrown during deserialization: {}", e.what());
            }

            return std::nullopt;
        }

        std::optional<Models::Page> GetLatest(LatestQueryOptions queryOptions) {
            WebUtils::URLOptions urlOptions {
                BEATSAVER_API_URL "/maps/latest",
                queryOptions.GetQueries()
            };

            try {
                auto response = downloader.Get<PageResponse>(urlOptions);
                if (response.IsSuccessful() && response.DataParsedSuccessful()) {
                    return response.responseData;
                }

                DEBUG("Response was unsuccesful (httpCode: {}, curlStatus: {}, parseSuccesful: {})", response.HttpCode, response.CurlStatus, response.DataParsedSuccessful());
            } catch (JsonException const& e) {
                ERROR("Json exception thrown during deserialization: {}", e.what());
            }

            return std::nullopt;
        }
    }

    namespace Users {
        void GetUserByIdAsync(int id, finished_opt_function<Models::UserDetail> onFinished) {
            if (!onFinished) return;

            std::thread([](int id, finished_opt_function<Models::UserDetail> onFinished){
                onFinished(GetUserById(id));
            }, id, std::forward<finished_opt_function<Models::UserDetail>>(onFinished)).detach();
        }

        void GetUsersByIdsAsync(std::span<int const> ids, finished_opt_function<std::unordered_map<int, Models::UserDetail>> onFinished) {
            if (!onFinished) return;

            std::thread([](std::vector<int> ids, finished_opt_function<std::unordered_map<int, Models::UserDetail>> onFinished){
                onFinished(GetUsersByIds(ids));
            }, std::vector<int>(ids.begin(), ids.end()), std::forward<finished_opt_function<std::unordered_map<int, Models::UserDetail>>>(onFinished)).detach();
        }

        void GetUserByNameAsync(std::string userName, finished_opt_function<Models::UserDetail> onFinished) {
            if (!onFinished) return;

            std::thread([](std::string userName, finished_opt_function<Models::UserDetail> onFinished){
                onFinished(GetUserByName(userName));
            }, std::forward<std::string>(userName), std::forward<finished_opt_function<Models::UserDetail>>(onFinished)).detach();
        }

        std::optional<Models::UserDetail> GetUserById(int id) {
            WebUtils::URLOptions urlOptions {
                fmt::format(BEATSAVER_API_URL "/users/id/{}", id)
            };
            try {
                auto response = downloader.Get<UserDetailResponse>(urlOptions);
                if (response.IsSuccessful() && response.DataParsedSuccessful()) {
                    return response.responseData;
                }

                DEBUG("Response was unsuccesful (httpCode: {}, curlStatus: {}, parseSuccesful: {})", response.HttpCode, response.CurlStatus, response.DataParsedSuccessful());
            } catch (JsonException const& e) {
                ERROR("Json exception thrown during deserialization: {}", e.what());
            }
            return std::nullopt;
        }

        std::optional<std::unordered_map<int, Models::UserDetail>> GetUsersByIds(std::span<int const> ids) {
            WebUtils::URLOptions urlOptions {
                fmt::format(BEATSAVER_API_URL "/users/ids/{}", fmt::join(ids.subspan(0, std::min<std::size_t>(50, ids.size())), ","))
            };
            try {
                auto response = downloader.Get<WebUtils::JsonResponse>(urlOptions);
                if (response.IsSuccessful() && response.DataParsedSuccessful()) {
                    auto& json = response.responseData.value();
                    ERROR_CHECK(json);

                    std::unordered_map<int, Models::UserDetail> parsedResults;
                    for (auto& value : json.GetArray()) {
                        auto detail = value.Get<Models::UserDetail>();
                        parsedResults.emplace(
                            detail.Id,
                            detail
                        );
                    }
                    return parsedResults;
                }

                DEBUG("Response was unsuccesful (httpCode: {}, curlStatus: {}, parseSuccesful: {})", response.HttpCode, response.CurlStatus, response.DataParsedSuccessful());
            } catch (JsonException const& e) {
                ERROR("Json exception thrown during deserialization: {}", e.what());
            }
            return std::nullopt;
        }

        std::optional<Models::UserDetail> GetUserByName(std::string userName) {
            WebUtils::URLOptions urlOptions {
                fmt::format(BEATSAVER_API_URL "/users/name/{}", userName)
            };
            try {
                auto response = downloader.Get<UserDetailResponse>(urlOptions);
                if (response.IsSuccessful() && response.DataParsedSuccessful()) {
                    return response.responseData;
                }

                DEBUG("Response was unsuccesful (httpCode: {}, curlStatus: {}, parseSuccesful: {})", response.HttpCode, response.CurlStatus, response.DataParsedSuccessful());
            } catch (JsonException const& e) {
                ERROR("Json exception thrown during deserialization: {}", e.what());
            }
            return std::nullopt;
        }
    }

    namespace Search {
        void GetPageAsync(int page, finished_opt_function<Models::Page> onFinished, SearchQueryOptions queryOptions) {
            if (!onFinished) return;

            std::thread([](int page, finished_opt_function<Models::Page> onFinished, SearchQueryOptions queryOptions){
                onFinished(GetPage(page, queryOptions));
            }, page, std::forward<finished_opt_function<Models::Page>>(onFinished), std::forward<SearchQueryOptions>(queryOptions)).detach();
        }

        std::optional<Models::Page> GetPage(int page, SearchQueryOptions queryOptions) {
            WebUtils::URLOptions urlOptions {
                fmt::format(BEATSAVER_API_URL "/search/text/{}", page),
                queryOptions.GetQueries()
            };

            try {
                auto response = downloader.Get<PageResponse>(urlOptions);
                if (response.IsSuccessful() && response.DataParsedSuccessful()) {
                    return response.responseData;
                }

                DEBUG("Response was unsuccesful (httpCode: {}, curlStatus: {}, parseSuccesful: {})", response.HttpCode, response.CurlStatus, response.DataParsedSuccessful());
            } catch (JsonException const& e) {
                ERROR("Json exception thrown during deserialization: {}", e.what());
            }

            return std::nullopt;
        }

        std::span<const std::string> GetMapFeelTags() {
            static std::string tags[] = {
                "Accuracy",
                "Balanced",
                "Challenge",
                "Dance",
                "Fitness",
                "Speed",
                "Tech"
            };
            return tags;
        }

        std::span<const std::string> GetGenreTags() {
            static std::string tags[] = {
                "Alternative",
                "Ambient",
                "Anime",
                "Classic & Orchestral",
                "Comedy & Meme",
                "Dance",
                "Drum and Bass",
                "Dubstep",
                "Electronic",
                "Folk & Acoustic",
                "Funk & Disco",
                "Hardcore",
                "Hip Hop & Rap",
                "Holiday",
                "House",
                "Indie",
                "Instrumental",
                "J-Pop",
                "J-Rock",
                "Jazz",
                "K-Pop",
                "Kids & Family",
                "Metal",
                "Nightcore",
                "Pop",
                "Punk",
                "R&B",
                "Rock",
                "Soul",
                "SpeedCore",
                "Swing",
                "TV & Film",
                "Techno",
                "Trance",
                "Video Game",
                "Vocaloid",
            };
            return tags;
        }
    }

    namespace Download {
        BeatmapDownloadInfo::BeatmapDownloadInfo(Models::Beatmap const& beatmap, Models::BeatmapVersion const& version) : Key(version.Key.value_or(beatmap.Id)), DownloadURL(version.DownloadURL), FolderName(fmt::format("{} ({} - {})", version.Key.value_or(beatmap.Id), beatmap.Metadata.SongName, beatmap.Metadata.LevelAuthorName)) {};

        std::future<std::optional<std::filesystem::path>> DownloadBeatmapAsync(BeatmapDownloadInfo downloadInfo) {
            return std::async(std::launch::any, &DownloadBeatmap, downloadInfo);
        }

        void DownloadBeatmapAsync(BeatmapDownloadInfo downloadInfo, finished_opt_function<std::filesystem::path> onFinished) {
            if (!onFinished) return;

            std::thread([](BeatmapDownloadInfo downloadInfo, finished_opt_function<std::filesystem::path> onFinished){
                onFinished(DownloadBeatmap(downloadInfo));
            }, std::forward<BeatmapDownloadInfo>(downloadInfo), std::forward<finished_opt_function<std::filesystem::path>>(onFinished)).detach();
        }

        std::future<std::unordered_map<std::string, std::optional<std::filesystem::path>>> DownloadBeatmapsAsync(std::span<BeatmapDownloadInfo const> infos) {
            static auto redirect = [](std::vector<BeatmapDownloadInfo> infos) {
                return DownloadBeatmaps(infos);
            };
            return std::async(std::launch::any, redirect, std::vector(infos.begin(), infos.end()));
        }

        void DownloadBeatmapsAsync(std::span<BeatmapDownloadInfo const> infos, finished_opt_function<std::unordered_map<std::string, std::optional<std::filesystem::path>>> onFinished) {
            if (!onFinished) return;

            std::thread([](std::vector<BeatmapDownloadInfo> downloadInfos, finished_opt_function<std::unordered_map<std::string, std::optional<std::filesystem::path>>> onFinished){
                onFinished(DownloadBeatmaps(downloadInfos));
            }, std::vector(infos.begin(), infos.end()), std::forward<finished_opt_function<std::unordered_map<std::string, std::optional<std::filesystem::path>>>>(onFinished)).detach();
        }

        void GetPreviewAsync(Models::BeatmapVersion const& beatmap, finished_opt_function<std::vector<uint8_t>> onFinished) {
            if (!onFinished) return;

            std::thread([](std::string dataURL, finished_opt_function<std::vector<uint8_t>> onFinished){
                onFinished(Utils::GetData(dataURL));
            }, std::forward<std::string>(beatmap.PreviewURL), std::forward<finished_opt_function<std::vector<uint8_t>>>(onFinished)).detach();
        }

        void GetCoverImageAsync(Models::BeatmapVersion const& beatmap, finished_opt_function<std::vector<uint8_t>> onFinished) {
            if (!onFinished) return;

            std::thread([](std::string dataURL, finished_opt_function<std::vector<uint8_t>> onFinished){
                onFinished(Utils::GetData(dataURL));
            }, std::forward<std::string>(beatmap.CoverURL), std::forward<finished_opt_function<std::vector<uint8_t>>>(onFinished)).detach();
        }

        std::optional<std::filesystem::path> DownloadBeatmap(BeatmapDownloadInfo downloadInfo) {
            auto targetPath = _defaultOutputRoothPath / Utils::ReplaceIllegalCharsInPath(downloadInfo.FolderName);

            if (BeatSaver::API::DownloadSongZip(downloadInfo.DownloadURL, targetPath)) {
                return targetPath;
            }

            return std::nullopt;
        }

        struct BulkBeatmapDownloadRequest : public WebUtils::IRequest {
            BulkBeatmapDownloadRequest(BeatmapDownloadInfo info) : url (info.DownloadURL), response(), info(info) {}
            WebUtils::URLOptions url;
            WebUtils::DataResponse response;
            BeatmapDownloadInfo info;

            virtual ~BulkBeatmapDownloadRequest() override = default;

            virtual WebUtils::IResponse* get_TargetResponse() override { return &response; }
            virtual WebUtils::IResponse const* get_TargetResponse() const override { return &response; }
            virtual WebUtils::URLOptions const& get_URL() const override { return url; }
        };

        std::unordered_map<std::string, std::optional<std::filesystem::path>> DownloadBeatmaps(std::span<BeatmapDownloadInfo const> infos) {
            std::unordered_map<std::string, std::optional<std::filesystem::path>> results;
            WebUtils::RatelimitedDispatcher rl;
            rl.rateLimitTime = std::chrono::milliseconds(1000);
            rl.maxConcurrentRequests = 4;

            rl.downloader = downloader;

            // request finished handler
            rl.onRequestFinished = [&results](bool success, WebUtils::IRequest* request) -> std::optional<WebUtils::RatelimitedDispatcher::RetryOptions> {
                if (!success) return WebUtils::RatelimitedDispatcher::RetryOptions(std::chrono::milliseconds(50));
                auto bulkReq = dynamic_cast<BulkBeatmapDownloadRequest*>(request);
                if (!bulkReq) return std::nullopt;

                auto& response = bulkReq->response;
                auto& info = bulkReq->info;
                results[info.Key] = std::nullopt;

                if (response.IsSuccessful() && response.DataParsedSuccessful()) {
                    auto outputPath = _defaultOutputRoothPath / Utils::ReplaceIllegalCharsInPath(info.FolderName);
                    if (Utils::ExtractAll(response.responseData.value(), outputPath)) {
                        results[info.Key] = outputPath;
                    }
                }
                return std::nullopt;
            };

            // create all the requests we want to run
            for (auto info : infos) {
                auto req = std::make_unique<BulkBeatmapDownloadRequest>(info);
                rl.AddRequest(std::move(req));
            }

            // start and wait for all requests to finish
            rl.StartDispatchIfNeeded().wait();
            return results;
        }

        std::optional<std::vector<uint8_t>> GetPreview(Models::BeatmapVersion const& beatmap) {
            return Utils::GetData(beatmap.PreviewURL);
        }

        std::optional<std::vector<uint8_t>> GetCoverImage(Models::BeatmapVersion const& beatmap) {
            return Utils::GetData(beatmap.CoverURL);
        }
    }
}
