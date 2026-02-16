#include "text_renderer.h"
#include "camera.h"
#include "../resource/resource_manager.h"
#include <SDL3_ttf/SDL_ttf.h>
#include <spdlog/spdlog.h>
#include <stdexcept>
engine::render::TextRenderer::TextRenderer(SDL_Renderer *sdl_renderer, engine::resource::ResourceManager *resource_manager)
    : sdl_renderer_(sdl_renderer), resource_manager_(resource_manager)
{
    if (!sdl_renderer_ || !resource_manager_)
    {
        throw std::runtime_error("TextRenderer init failed");
    }
    if (!TTF_WasInit() && TTF_Init() == false)
    {
        throw std::runtime_error("TTF_Init failed");
    }
    text_engine_ = TTF_CreateRendererTextEngine(sdl_renderer_);
    if (!text_engine_)
    {
        spdlog::error("TTF_CreateRendererTextEngine failed");
        throw std::runtime_error("TTF_CreateRendererTextEngine failed");
    }
    spdlog::info("TextRenderer init successfully");
}

engine::render::TextRenderer::~TextRenderer()
{
    if (text_engine_)
    {
        close();
    }
}

void engine::render::TextRenderer::close()
{
    if (text_engine_)
    {
        TTF_DestroyRendererTextEngine(text_engine_);
        text_engine_ = nullptr;
        spdlog::info("TextRenderer close successfully");
    }
    TTF_Quit();
}

void engine::render::TextRenderer::drawUIText(const std::string &text, entt::id_type font_id, int font_size, const glm::vec2 &position, const std::string &font_path, const engine::utils::FColor &color)
{
    TTF_Font *font = resource_manager_->getFont(font_id, font_size, font_path);
    if (!font)
    {
        spdlog::warn("Font not found: {} - {}", font_id, font_size);
        return;
    }
    TTF_Text *temp_text_object = TTF_CreateText(text_engine_, font, text.c_str(), 0);
    if (!temp_text_object)
    {
        spdlog::error("TTF_CreateText failed");
        return;
    }
    TTF_SetTextColorFloat(temp_text_object, 0.0f, 0.0f, 0.0f, 1.0f);
    if (!TTF_DrawRendererText(temp_text_object, position.x + 2, position.y + 2))
    {
        spdlog::error("TTF_DrawRendererText failed", SDL_GetError());
    }
    TTF_SetTextColorFloat(temp_text_object, color.r, color.g, color.b, color.a);
    if (!TTF_DrawRendererText(temp_text_object, position.x, position.y))
    {
        spdlog::error("drawUIText 绘制临时 TTF_Text 失败: {}", SDL_GetError());
    }
    TTF_DestroyText(temp_text_object);
}

void engine::render::TextRenderer::drawText(const Camera &camera, const std::string &text, entt::id_type font_id, int font_size, const glm::vec2 &position, const engine::utils::FColor &color)
{
    glm::vec2 position_screen = camera.world2Screen(position);
    drawUIText(text, font_id, font_size, position_screen, "", color);
}

glm::vec2 engine::render::TextRenderer::getTextSize(const std::string &text, entt::id_type font_id, int font_size, const std::string &font_path)
{
    TTF_Font *font = resource_manager_->getFont(font_id, font_size, font_path);
    if (!font)
    {
        spdlog::warn("Font not found: {} - {}", font_id, font_size);
        return glm::vec2{0, 0};
    }

    TTF_Text *temp_text_object = TTF_CreateText(text_engine_, font, text.c_str(), 0);
    if (!temp_text_object)
    {
        spdlog::error("TTF_CreateText failed");
        return glm::vec2{0, 0};
    }

    int width, height;
    TTF_GetTextSize(temp_text_object, &width, &height);
    TTF_DestroyText(temp_text_object);
    return glm::vec2(static_cast<float>(width), static_cast<float>(height));
}
