#include "texture_manager.h"
#include <SDL3_image/SDL_image.h>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <entt/core/hashed_string.hpp>
engine::resource::TextureManager::TextureManager(SDL_Renderer *renderer)
    : renderer_(renderer)
{
    if (!renderer_)
    {
        throw std::runtime_error("TextureManager init failed");
    }
    spdlog::info("TextureManager init successfully");
}

SDL_Texture *engine::resource::TextureManager::loadTexture(entt::id_type id, const std::string &file_path)
{
    auto it = textures_.find(id);
    if (it != textures_.end())
    {
        return it->second.get();
    }
    SDL_Texture *raw_texture = IMG_LoadTexture(renderer_, file_path.data());

    if (!SDL_SetTextureScaleMode(raw_texture, SDL_SCALEMODE_NEAREST))
    {
        /* code */
        spdlog::warn("Set texture scale mode failed: {}", file_path);
    }

    if (!raw_texture)
    {
        spdlog::error("Load texture failed: {} , SDL error: {}", file_path, SDL_GetError());
        return nullptr;
    }

    textures_.emplace(id, std::unique_ptr<SDL_Texture, SDLTextureDeleter>(raw_texture));
    spdlog::info("Load texture successfully: {}", file_path);

    return raw_texture;
}

SDL_Texture *engine::resource::TextureManager::loadTexture(entt::hashed_string str_hs)
{
    return loadTexture(str_hs.value(), str_hs.data());
}

SDL_Texture *engine::resource::TextureManager::getTexture(entt::id_type id, const std::string &file_path)
{
    auto it = textures_.find(id);
    if (it != textures_.end())
    {
        return it->second.get();
    }
    spdlog::warn("Texture not found: {}", file_path);
    return loadTexture(id, file_path);
}

SDL_Texture *engine::resource::TextureManager::getTexture(entt::hashed_string str_hs)
{
    return getTexture(str_hs.value(), str_hs.data());
}

glm::vec2 engine::resource::TextureManager::getTextureSize(entt::id_type id, const std::string &file_path)
{
    SDL_Texture *texture = getTexture(id, file_path);
    if (!texture)
    {
        spdlog::error("Texture not found: {}", file_path.data());
        return glm::vec2(0.0f, 0.0f);
    }

    glm::vec2 size;
    if (!SDL_GetTextureSize(texture, &size.x, &size.y))
    {
        spdlog::error("Get texture size failed: {}", file_path.data());
        return glm::vec2(0.0f, 0.0f);
    }
    return size;
}

glm::vec2 engine::resource::TextureManager::getTextureSize(entt::hashed_string str_hs)
{
    return getTextureSize(str_hs.value(), str_hs.data());
}

void engine::resource::TextureManager::unloadTexture(entt::id_type id)
{
    auto it = textures_.find(id);
    if (it != textures_.end())
    {
        spdlog::info("Unload texture id: {}", id);
        textures_.erase(it);
    }
    else
    {
        spdlog::warn("Texture id not found: {}", id);
    }
}

void engine::resource::TextureManager::clearTextures()
{
    if (!textures_.empty())
    {
        /* code */
        spdlog::info("Clear {} textures", textures_.size());
        textures_.clear();
    }
}
