#include "render.h"
#include "../resource/resource_manager.h"
#include "camera.h"
#include <spdlog/spdlog.h>
#include <SDL3/SDL.h>
#include <stdexcept>
engine::render::Renderer::Renderer(SDL_Renderer *sdl_renderer, engine::resource::ResourceManager *resource_manager)
    : renderer_(sdl_renderer), resource_manager_(resource_manager)
{
    spdlog::info("Renderer init ...");
    if (!renderer_)
    {
        throw std::runtime_error("Renderer init failed");
    }
    if (!resource_manager_)
    {
        throw std::runtime_error("Renderer init failed");
    }

    spdlog::info("Renderer init successfully");
}

void engine::render::Renderer::drawSprite(const Camera &camera, const engine::render::Sprite &sprite, const glm::vec2 &position, const glm::vec2 &scale, double angle)
{
    auto texture = resource_manager_->getTexture(sprite.getTextureId());
    if (!texture)
    {
        spdlog::error("Texture not found:{}", sprite.getTextureId());
        return;
    }
    auto src_rect = getSpriteSrcRect(sprite);
    if (!src_rect.has_value())
    {
        spdlog::error("Invalid source rectangle:{}", sprite.getTextureId());
        return;
    }

    glm::vec2 position_screen = camera.world2Screen(position);
    float scale_w = src_rect.value().w * scale.x;
    float scale_h = src_rect.value().h * scale.y;
    SDL_FRect dest_rect = {position_screen.x, position_screen.y, scale_w, scale_h};
    if (!isRectInViewport(camera, dest_rect))
    {
        return;
    }

    if (!SDL_RenderTextureRotated(renderer_, texture, &src_rect.value(), &dest_rect, angle, NULL, sprite.isFlipped() ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE))
    {
        spdlog::error("Render texture failed:{},{}", sprite.getTextureId(), SDL_GetError());
    }
}

void engine::render::Renderer::drawParallx(const Camera &camera, const engine::render::Sprite &sprite, const glm::vec2 &position, const glm::vec2 &scroll_factor, const glm::bvec2 &repeat, const glm::vec2 &scale)
{
    auto texture = resource_manager_->getTexture(sprite.getTextureId());
    if (!texture)
    {
        spdlog::error("Texture not found:{}", sprite.getTextureId());
        return;
    }
    auto src_rect = getSpriteSrcRect(sprite);
    if (!src_rect.has_value())
    {
        spdlog::error("Invalid source rectangle:{}", sprite.getTextureId());
        return;
    }
    glm::vec2 position_screen = camera.world2ScreenWithParallax(position, scroll_factor);
    float scale_w = src_rect.value().w * scale.x;
    float scale_h = src_rect.value().h * scale.y;
    glm::vec2 start;
    glm::vec2 stop;
    glm::vec2 viewport_size = camera.getViewportSize();
    if (repeat.x)
    {
        start.x = glm::mod(position_screen.x, scale_w) - scale_w;
        stop.x = viewport_size.x;
    }
    else
    {
        start.x = position_screen.x;
        stop.x = glm::min(position_screen.x + scale_w, viewport_size.x); // 结束点是一个纹理宽度之后，但不超过视口宽度
    }
    if (repeat.y)
    {
        start.y = glm::mod(position_screen.y, scale_h) - scale_h;
        stop.y = viewport_size.y;
    }
    else
    {
        start.y = position_screen.y;
        stop.y = glm::min(position_screen.y + scale_h, viewport_size.y);
    }

    for (float y = start.y; y < stop.y; y += scale_h)
    {
        for (float x = start.x; x < stop.x; x += scale_w)
        {
            SDL_FRect dest_rect = {x, y, scale_w, scale_h};
            if (!SDL_RenderTexture(renderer_, texture, nullptr, &dest_rect))
            {
                spdlog::error("Render texture failed:{},{}", sprite.getTextureId(), SDL_GetError());
                return;
            }
        }
    }
}

void engine::render::Renderer::drawUISprite(const engine::render::Sprite &sprite, const glm::vec2 &position, const std::optional<glm::vec2> &size)
{
    auto texture = resource_manager_->getTexture(sprite.getTextureId());
    if (!texture)
    {
        spdlog::error("Texture not found:{}", sprite.getTextureId());
        return;
    }
    auto src_rct = getSpriteSrcRect(sprite);
    if (!src_rct.has_value())
    {
        spdlog::error("Invalid source rectangle:{}", sprite.getTextureId());
        return;
    }
    SDL_FRect dest_rect = {position.x, position.y, 0, 0};
    if (size.has_value())
    {
        dest_rect.w = size.value().x;
        dest_rect.h = size.value().y;
    }
    else
    {
        dest_rect.w = src_rct.value().w;
        dest_rect.h = src_rct.value().h;
    }
    if (!SDL_RenderTextureRotated(renderer_, texture, &src_rct.value(), &dest_rect, 0, nullptr, sprite.isFlipped() ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE))
    {
        spdlog::error("Render texture failed:{},{}", sprite.getTextureId(), SDL_GetError());
    }
}

void engine::render::Renderer::drawUIFillRect(const engine::utils::Rect &rect, const engine::utils::FColor &color)
{
    setDrawColorFloat(color.r, color.g, color.b, color.a);
    SDL_FRect sdl_rect = {rect.position.x, rect.position.y, rect.size.x, rect.size.y};
    if (!SDL_RenderFillRect(renderer_, &sdl_rect))
    {
        spdlog::error("Render fill rect failed:{}", SDL_GetError());
    }
    setDrawColorFloat(0.0f, 0.0f, 0.0f, 1.0f);
}

void engine::render::Renderer::present()
{
    SDL_RenderPresent(renderer_);
}

void engine::render::Renderer::clearScreen()
{
    if (!SDL_RenderClear(renderer_))
    {
        spdlog::error("Render clear failed:{}", SDL_GetError());
    }
}

void engine::render::Renderer::setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    if (!SDL_SetRenderDrawColor(renderer_, r, g, b, a))
    {
        spdlog::error("Set render draw color failed:{}", SDL_GetError());
    }
}

void engine::render::Renderer::setDrawColorFloat(float r, float g, float b, float a)
{
    if (!SDL_SetRenderDrawColorFloat(renderer_, r, g, b, a))
    {
        spdlog::error("Set render draw color failed:{}", SDL_GetError());
    }
}

std::optional<SDL_FRect> engine::render::Renderer::getSpriteSrcRect(const Sprite &sprite)
{
    SDL_Texture *texture = resource_manager_->getTexture(sprite.getTextureId());
    if (!texture)
    {
        spdlog::error("Texture not found: {}", sprite.getTextureId());
        return std::nullopt;
    }
    auto src_rect = sprite.getSourceRect();
    if (src_rect.has_value())
    {
        const auto &rect = src_rect.value();
        if (rect.size.x <= 0 || rect.size.y <= 0)
        {
            spdlog::error("Invalid source rectangle: texture={}, rect=({},{},{},{})",
                          sprite.getTextureId(), rect.position.x, rect.position.y, rect.size.x, rect.size.y);
            return std::nullopt;
        }
        // 将 SDL_Rect (int) 转换为 SDL_FRect (float)
        return SDL_FRect{
            static_cast<float>(rect.position.x),
            static_cast<float>(rect.position.y),
            static_cast<float>(rect.size.x),
            static_cast<float>(rect.size.y)};
    }
    else
    {
        SDL_FRect result = {0, 0, 0, 0};
        if (!SDL_GetTextureSize(texture, &result.w, &result.h))
        {
            spdlog::error("Get texture size failed:{}", sprite.getTextureId());
            return std::nullopt;
        }
        return result;
    }
}

bool engine::render::Renderer::isRectInViewport(const Camera &camera, const SDL_FRect &rect)
{
    glm::vec2 viewport_size = camera.getViewportSize();
    return rect.x + rect.w >= 0 && rect.x <= viewport_size.x && rect.y + rect.h >= 0 && rect.y <= viewport_size.y;
}
