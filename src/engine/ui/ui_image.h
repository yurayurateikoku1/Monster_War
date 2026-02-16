#pragma once
#include "ui_element.h"
#include "../render/sprite.h"
#include <string>
#include <string_view>
#include <optional>
#include <SDL3/SDL_rect.h>

namespace engine::ui
{
    class UIImage final : public UIElement
    {
    protected:
        engine::render::Sprite sprite_;

    public:
        UIImage(const std::string &texture_path, const glm::vec2 &position = {0.0f, 0.0f}, const glm::vec2 &size = {0.0f, 0.0f},
                const std::optional<engine::utils::Rect> &source_rect = std::nullopt, bool is_flipped = false);
        UIImage(entt::id_type texture_id,
                glm::vec2 position = {0.0f, 0.0f},
                glm::vec2 size = {0.0f, 0.0f},
                std::optional<engine::utils::Rect> source_rect = std::nullopt,
                bool is_flipped = false);
        UIImage(engine::render::Sprite &image,
                glm::vec2 position = {0.0f, 0.0f},
                glm::vec2 size = {0.0f, 0.0f});

        void render(engine::core::Context &context) override;

        const engine::render::Sprite &getSprite() const { return sprite_; }
        void setSprite(engine::render::Sprite sprite) { sprite_ = std::move(sprite); }

        std::string getTexturePath() const { return sprite_.getTexturePath(); }
        entt::id_type getTextureId() const { return sprite_.getTextureId(); }
        void setTextureId(const std::string &texture_path) { sprite_.setTexture(texture_path); }

        const std::optional<engine::utils::Rect> &getSourceRect() const { return sprite_.getSourceRect(); }
        void setSourceRect(std::optional<engine::utils::Rect> source_rect) { sprite_.setSourceRect(std::move(source_rect)); }

        bool isFlipped() const { return sprite_.isFlipped(); }
        void setFlipped(bool flipped) { sprite_.setFlipped(flipped); }
    };
}