#pragma once
#include <SDL3/SDL_render.h>
#include <string>
#include <glm/vec2.hpp>
#include "../utils/math.h"
#include <entt/core/hashed_string.hpp>
struct TTF_TextEngine;

namespace engine::resource
{
    class ResourceManager;
}

namespace engine::render
{
    class Camera;
    class TextRenderer final
    {
    private:
        SDL_Renderer *sdl_renderer_ = nullptr;
        engine::resource::ResourceManager *resource_manager_ = nullptr;
        TTF_TextEngine *text_engine_ = nullptr;

    public:
        TextRenderer(SDL_Renderer *sdl_renderer, engine::resource::ResourceManager *resource_manager);
        ~TextRenderer();
        TextRenderer(const TextRenderer &) = delete;
        TextRenderer(TextRenderer &&) = delete;
        TextRenderer &operator=(const TextRenderer &) = delete;
        TextRenderer &operator=(TextRenderer &&) = delete;

        void close();
        void drawUIText(const std::string &text, entt::id_type font_id, int font_size,
                        const glm::vec2 &position, const std::string &font_path = "",
                        const engine::utils::FColor &color = {1.0f, 1.0f, 1.0f, 1.0f});
        void drawText(const Camera &camera, const std::string &text, entt::id_type font_id, int font_size,
                      const glm::vec2 &position, const engine::utils::FColor &color = {1.0f, 1.0f, 1.0f, 1.0f});
        glm::vec2 getTextSize(const std::string &text, entt::id_type font_id, int font_size, const std::string &font_path = "");
    };
}
