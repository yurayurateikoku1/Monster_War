#pragma once
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include <entt/core/fwd.hpp>
struct SDL_Renderer;
struct SDL_Texture;
struct MIX_Audio;
struct MIX_Mixer;
struct TTF_Font;

namespace engine::resource
{
    class TextureManager;
    class AudioManager;
    class FontManager;

    /// @brief 资源管理器中央控制器
    class ResourceManager
    {
    private:
        std::unique_ptr<TextureManager> texture_manager_{nullptr};
        std::unique_ptr<AudioManager> audio_manager_{nullptr};
        std::unique_ptr<FontManager> font_manager_{nullptr};

    public:
        explicit ResourceManager(SDL_Renderer *renderer);

        ~ResourceManager();
        ResourceManager(const ResourceManager &) = delete;
        ResourceManager(ResourceManager &&) = delete;
        ResourceManager &operator=(const ResourceManager &) = delete;
        ResourceManager &operator=(ResourceManager &&) = delete;

        void clear();

        MIX_Mixer *getMixer() const;

        //=====Texture=====
        SDL_Texture *loadTexture(entt::id_type id, const std::string &file_path);
        SDL_Texture *loadTexture(entt::hashed_string str_hs);
        SDL_Texture *getTexture(entt::id_type id, const std::string &file_path = "");
        SDL_Texture *getTexture(entt::hashed_string str_hs);
        void unloadTexture(entt::id_type id);
        glm::vec2 getTextureSize(entt::id_type id, const std::string &file_path = "");
        glm::vec2 getTextureSize(entt::hashed_string str_hs);
        void clearTextures();

        //=====Sound=====
        MIX_Audio *loadSound(entt::id_type id, const std::string &file_path);
        MIX_Audio *loadSound(entt::hashed_string str_hs);
        MIX_Audio *getSound(entt::id_type id, const std::string &file_path = "");
        MIX_Audio *getSound(entt::hashed_string str_hs);
        void unloadSound(entt::id_type id);
        void clearSounds();

        //=====Music=====
        MIX_Audio *loadMusic(entt::id_type id, const std::string &file_path);
        MIX_Audio *loadMusic(entt::hashed_string str_hs);
        MIX_Audio *getMusic(entt::id_type id, const std::string &file_path = "");
        MIX_Audio *getMusic(entt::hashed_string str_hs);
        void unloadMusic(entt::id_type id);
        void clearMusics();

        //=====Font=====
        TTF_Font *loadFont(entt::id_type id, const std::string &file_path, int font_size);
        TTF_Font *loadFont(entt::hashed_string str_hs, int font_size);
        TTF_Font *getFont(entt::id_type id, int font_size, const std::string &file_path = "");
        TTF_Font *getFont(entt::hashed_string str_hs, int font_size);
        void unloadFont(entt::id_type id, int font_size);
        void clearFonts();
    };
}