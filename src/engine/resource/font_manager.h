#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <SDL3_ttf/SDL_ttf.h>
#include <utility>
#include <functional>
#include <glm/glm.hpp>
#include <entt/core/fwd.hpp>
namespace engine::resource
{
    using FontKey = std::pair<entt::id_type, int>;
    struct FontKeyHash
    {
        std::size_t operator()(const FontKey &key) const noexcept
        {
            // 采用C++20标准库的hash_combine实现思路
            std::size_t h1 = std::hash<entt::id_type>{}(key.first);
            std::size_t h2 = std::hash<int>{}(key.second);
            //
            return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
        }
    };

    /// @brief 字体管理器
    class FontManager
    {
        friend class ResourceManager;

    public:
        FontManager();
        ~FontManager();
        FontManager(const FontManager &) = delete;
        FontManager(FontManager &&) = delete;
        FontManager &operator=(const FontManager &) = delete;
        FontManager &operator=(FontManager &&) = delete;

    private:
        struct SDLFontDeleter
        {
            /* data */
            void operator()(TTF_Font *font) const
            {
                if (font)
                {
                    TTF_CloseFont(font);
                }
            }
        };

        SDL_Renderer *renderer_ = nullptr;
        std::unordered_map<FontKey, std::unique_ptr<TTF_Font, SDLFontDeleter>, FontKeyHash> fonts_;

        TTF_Font *loadFont(entt::id_type id, int point_size, const std::string &file_path);

        TTF_Font *loadFont(entt::hashed_string str_hs, int point_size);

        TTF_Font *getFont(entt::id_type id, int point_size, const std::string &file_path = "");

        TTF_Font *getFont(entt::hashed_string str_hs, int point_size);

        void unloadFont(entt::id_type id, int point_size);
        void clearFonts();
    };
}