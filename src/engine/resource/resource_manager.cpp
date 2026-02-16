#include "resource_manager.h"
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>
#include "texture_manager.h"
#include "audio_manager.h"
#include "font_manager.h"
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <entt/core/hashed_string.hpp>
engine::resource::ResourceManager::ResourceManager(SDL_Renderer *renderer)
{
    texture_manager_ = std::make_unique<TextureManager>(renderer);
    audio_manager_ = std::make_unique<AudioManager>();
    font_manager_ = std::make_unique<FontManager>();

    spdlog::info("ResourceManager init successfully");
}

engine::resource::ResourceManager::~ResourceManager() = default;

MIX_Mixer *engine::resource::ResourceManager::getMixer() const
{
    return audio_manager_->getMixer();
}

void engine::resource::ResourceManager::loadResource(const std::string &file_path)
{
    std::filesystem::path path(file_path);
    if (!std::filesystem::exists(path))
    {
        spdlog::error("File not found: {}", file_path);
        return;
    }
    std::ifstream file(file_path);
    nlohmann::json json;
    file >> json;
    try
    {
        if (json.contains("sound"))
        {
            for (const auto &[key, value] : json["sound"].items())
            {
                loadSound(entt::hashed_string(key.c_str()), value.get<std::string>());
            }
        }
        if (json.contains("music"))
        {
            for (const auto &[key, value] : json["music"].items())
            {
                loadMusic(entt::hashed_string(key.c_str()), value.get<std::string>());
            }
        }
        if (json.contains("texture"))
        {
            for (const auto &[key, value] : json["texture"].items())
            {
                loadTexture(entt::hashed_string(key.c_str()), value.get<std::string>());
            }
        }
        if (json.contains("font"))
        {
            for (const auto &[key, value] : json["font"].items())
            {
                loadFont(entt::hashed_string(key.c_str()), value.get<std::string>(), value.get<int>());
            }
        }
    }
    catch (const std::exception &e)
    {
        spdlog::error("Failed to load resource: {},{},{},{}", file_path, e.what(), __FILE__, __LINE__);
    }
}

void engine::resource::ResourceManager::clear()
{
    texture_manager_->clearTextures();
    audio_manager_->clearSounds();
    audio_manager_->clearMusics();
    font_manager_->clearFonts();
}

SDL_Texture *engine::resource::ResourceManager::loadTexture(entt::id_type id, const std::string &file_path)
{
    return texture_manager_->loadTexture(id, file_path);
}

SDL_Texture *engine::resource::ResourceManager::loadTexture(entt::hashed_string str_hs)
{
    return texture_manager_->loadTexture(str_hs);
}

SDL_Texture *engine::resource::ResourceManager::getTexture(entt::id_type id, const std::string &file_path)
{
    return texture_manager_->getTexture(id, file_path);
}

SDL_Texture *engine::resource::ResourceManager::getTexture(entt::hashed_string str_hs)
{
    return texture_manager_->getTexture(str_hs);
}
void engine::resource::ResourceManager::unloadTexture(entt::id_type id)
{
    texture_manager_->unloadTexture(id);
}

glm::vec2 engine::resource::ResourceManager::getTextureSize(entt::hashed_string str_hs)
{
    return texture_manager_->getTextureSize(str_hs);
}

glm::vec2 engine::resource::ResourceManager::getTextureSize(entt::id_type id, const std::string &file_path)
{
    return texture_manager_->getTextureSize(id, file_path);
}
void engine::resource::ResourceManager::clearTextures()
{
    texture_manager_->clearTextures();
}

MIX_Audio *engine::resource::ResourceManager::loadSound(entt::id_type id, const std::string &file_path)
{
    return audio_manager_->loadSound(id, file_path);
}

MIX_Audio *engine::resource::ResourceManager::loadSound(entt::hashed_string str_hs)
{
    return audio_manager_->loadSound(str_hs);
}

MIX_Audio *engine::resource::ResourceManager::getSound(entt::hashed_string str_hs)
{
    return audio_manager_->getSound(str_hs);
}

MIX_Audio *engine::resource::ResourceManager::getSound(entt::id_type id, const std::string &file_path)
{
    return audio_manager_->getSound(id, file_path);
}

void engine::resource::ResourceManager::unloadSound(entt::id_type id)
{
    audio_manager_->unloadSound(id);
}

void engine::resource::ResourceManager::clearSounds()
{
    audio_manager_->clearSounds();
}

MIX_Audio *engine::resource::ResourceManager::loadMusic(entt::hashed_string str_hs)
{
    return audio_manager_->loadMusic(str_hs);
}

MIX_Audio *engine::resource::ResourceManager::getMusic(entt::hashed_string str_hs)
{
    return audio_manager_->getMusic(str_hs);
}

MIX_Audio *engine::resource::ResourceManager::loadMusic(entt::id_type id, const std::string &file_path)
{
    return audio_manager_->loadMusic(id, file_path);
}

MIX_Audio *engine::resource::ResourceManager::getMusic(entt::id_type id, const std::string &file_path)
{
    return audio_manager_->getMusic(id, file_path);
}

void engine::resource::ResourceManager::unloadMusic(entt::id_type id)
{
    audio_manager_->unloadMusic(id);
}

void engine::resource::ResourceManager::clearMusics()
{
    audio_manager_->clearMusics();
}

TTF_Font *engine::resource::ResourceManager::loadFont(entt::hashed_string str_hs, int font_size)
{
    return font_manager_->loadFont(str_hs, font_size);
}

TTF_Font *engine::resource::ResourceManager::getFont(entt::hashed_string str_hs, int font_size)
{
    return font_manager_->getFont(str_hs, font_size);
}

TTF_Font *engine::resource::ResourceManager::loadFont(entt::id_type id, const std::string &file_path, int font_size)
{
    return font_manager_->loadFont(id, font_size, file_path);
}

TTF_Font *engine::resource::ResourceManager::getFont(entt::id_type id, int font_size, const std::string &file_path)
{
    return font_manager_->getFont(id, font_size, file_path);
}

void engine::resource::ResourceManager::unloadFont(entt::id_type id, int font_size)
{
    font_manager_->unloadFont(id, font_size);
}

void engine::resource::ResourceManager::clearFonts()
{
    font_manager_->clearFonts();
}