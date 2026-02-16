#include "config.h"
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <fstream>
engine::core::Config::Config(const std::string &file_path)
{
    loadFromFile(file_path);
}

bool engine::core::Config::loadFromFile(const std::string &file_path)
{
    std::ifstream file(file_path);
    if (!file.is_open())
    {
        spdlog::warn("Config file not found: {}", file_path);

        if (!save2File(file_path))
        {
            spdlog::error("Failed to save config file: {}", file_path);
            return false;
        }
        return false;
    }
    try
    {
        nlohmann::json j;
        file >> j;
        fromJson(j);
        spdlog::info("Config file loaded: {}", file_path);
        return true;
    }
    catch (const std::exception &e)
    {
        spdlog::error("Failed to save config file: {},{},{},{}", file_path, e.what(), __FILE__, __LINE__);
    }
    return false;
}

bool engine::core::Config::save2File(const std::string &file_path)
{
    std::ofstream file(file_path);
    if (!file.is_open())
    {
        spdlog::error("Failed to save config file: {}", file_path);
        return false;
    }
    try
    {
        nlohmann::ordered_json j = toJson();
        file << j.dump(4);
        spdlog::info("Saved config file: {}", file_path);
        return true;
    }
    catch (const std::exception &e)
    {
        spdlog::error("Failed to save config file: {},{},{},{}", file_path, e.what(), __FILE__, __LINE__);
    }
    return false;
}

void engine::core::Config::fromJson(const nlohmann::json &j)
{
    if (j.contains("window"))
    {
        const auto &window_config = j["window"];
        window_title_ = window_config.value("title", window_title_);
        window_width_ = window_config.value("width", window_width_);
        window_height_ = window_config.value("height", window_height_);
        window_scale_ = window_config.value("window_scale", window_scale_);
        window_logical_scale_ = window_config.value("logical_scale", window_logical_scale_);
        window_resizable_ = window_config.value("resizable", window_resizable_);
    }
    if (j.contains("graphics"))
    {
        const auto &graphics = j["graphics"];
        vsync_enabled_ = graphics.value("vsync", vsync_enabled_);
    }
    if (j.contains("performance"))
    {
        /* code */
        const auto &perf_config = j["performance"];
        target_fps_ = perf_config.value("target_fps", target_fps_);
        if (target_fps_ < 0)
        {
            spdlog::warn("Target FPS must be greater than 0");
            target_fps_ = 0;
        }
    }
    if (j.contains("audio"))
    {
        const auto &audio_config = j["audio"];
        music_volume_ = audio_config.value("music_volume", music_volume_);
        sound_volume_ = audio_config.value("sound_volume", sound_volume_);
    }
    if (j.contains("input_mappings") && j["input_mappings"].is_object())
    {
        const auto mappings_json = j["input_mappings"];
        try
        {
            auto input_mappings = mappings_json.get<std::unordered_map<std::string, std::vector<std::string>>>();
            _input_mappings = std::move(input_mappings);
            spdlog::info("Input mappings loaded");
        }
        catch (const std::exception &e)
        {
            spdlog::warn("Use default settings,Failed to load input mappings: {},", e.what());
        }
    }
    else
    {
        spdlog::warn("Use default settings,Failed to load input mappings");
    }
}

nlohmann::ordered_json engine::core::Config::toJson() const
{

    return nlohmann::ordered_json{
        {
            "window",
            {
                {"title", window_title_},
                {"width", window_width_},
                {"height", window_height_},
                {"window_scale", window_scale_},
                {"logical_scale", window_logical_scale_},
                {"resizable", window_resizable_},
            },
        },
        {
            "graphics",
            {
                {"vsync", vsync_enabled_},
            },
        },
        {
            "performance",
            {
                {"target_fps", target_fps_},
            },
        },
        {
            "audio",
            {
                {"music_volume", music_volume_},
                {"sound_volume", sound_volume_},
            },
        },
        {"input_mappings",
         _input_mappings}};
}
