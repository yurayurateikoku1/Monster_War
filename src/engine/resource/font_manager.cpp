#include "font_manager.h"
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <entt/core/hashed_string.hpp>
engine::resource::FontManager::FontManager()
{
    if (!TTF_WasInit() && !TTF_Init())
    {
        /* code */
        throw std::runtime_error("FontManager init failed");
    }
    spdlog::info("FontManager init successfully");
}

engine::resource::FontManager::~FontManager()
{
    if (!fonts_.empty())
    {
        spdlog::info("FontManager not empty, dmynically clear fonts");
        clearFonts();
    }
    TTF_Quit();
    spdlog::info("FontManager quit successfully");
}

TTF_Font *engine::resource::FontManager::loadFont(entt::id_type id, int point_size, const std::string &file_path)
{
    if (point_size <= 0)
    {
        spdlog::error("Font size must be greater than 0");
        return nullptr;
    }
    FontKey key(id, point_size);
    auto it = fonts_.find(key);
    if (it != fonts_.end())
    {
        return it->second.get();
    }
    spdlog::info("Load font: {} - {}", id, point_size);
    TTF_Font *raw_font = TTF_OpenFont(file_path.data(), point_size);
    if (!raw_font)
    {
        spdlog::error("Failed to load font: {} - {}", id, point_size);
        return nullptr;
    }
    fonts_.emplace(key, std::unique_ptr<TTF_Font, SDLFontDeleter>(raw_font));
    spdlog::info("Load font successfully: {} - {}", file_path.data(), point_size);
    return raw_font;
}
TTF_Font *engine::resource::FontManager::loadFont(entt::hashed_string str_hs, int point_size)
{
    return loadFont(str_hs.value(), point_size, str_hs.data());
}
void engine::resource::FontManager::unloadFont(entt::id_type id, int point_size)
{
    FontKey key(id, point_size);
    auto it = fonts_.find(key);
    if (it != fonts_.end())
    {
        spdlog::info("Unload font: {} - {}", id, point_size);
        fonts_.erase(it);
        spdlog::info("Unload font successfully: {} - {}", id, point_size);
    }
    else
    {
        spdlog::warn("Font not found: {} - {},Unload font failed", id, point_size);
    }
}

TTF_Font *engine::resource::FontManager::getFont(entt::id_type id, int point_size, const std::string &file_path)
{
    FontKey key(id, point_size);
    auto it = fonts_.find(key);
    if (it != fonts_.end())
    {
        return it->second.get();
    }
    spdlog::warn("Font not found: {} - {}", id, point_size);

    return loadFont(id, point_size, file_path);
}

TTF_Font *engine::resource::FontManager::getFont(entt::hashed_string str_hs, int point_size)
{
    return getFont(str_hs.value(), point_size, str_hs.data());
}

void engine::resource::FontManager::clearFonts()
{
    if (!fonts_.empty())
    {
        /* code */
        spdlog::info("Clear {} fonts", fonts_.size());
        fonts_.clear();
    }
}
