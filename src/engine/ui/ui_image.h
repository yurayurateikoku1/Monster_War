#pragma once
#include "ui_element.h"
#include "../render/image.h"
#include <string>
#include <string_view>
#include <optional>
#include <SDL3/SDL_rect.h>

namespace engine::ui
{
    class UIImage final : public UIElement
    {
    protected:
        engine::render::Image image_;

    public:
        UIImage(const std::string &texture_path, const glm::vec2 &position = {0.0f, 0.0f}, const glm::vec2 &size = {0.0f, 0.0f},
                const std::optional<engine::utils::Rect> &source_rect = std::nullopt, bool is_flipped = false);
        UIImage(entt::id_type texture_id,
                glm::vec2 position = {0.0f, 0.0f},
                glm::vec2 size = {0.0f, 0.0f},
                std::optional<engine::utils::Rect> source_rect = std::nullopt,
                bool is_flipped = false);
        UIImage(engine::render::Image &image,
                glm::vec2 position = {0.0f, 0.0f},
                glm::vec2 size = {0.0f, 0.0f});

        void render(engine::core::Context &context) override;

        const engine::render::Image &getImage() const { return image_; }
        void setImage(engine::render::Image image) { image_ = std::move(image); }

        std::string getTexturePath() const { return image_.getTexturePath(); }
        entt::id_type getTextureId() const { return image_.getTextureId(); }
        void setTextureId(const std::string &texture_path) { image_.setTexture(texture_path); }

        const std::optional<engine::utils::Rect> &getSourceRect() const { return image_.getSourceRect(); }
        void setSourceRect(std::optional<engine::utils::Rect> source_rect) { image_.setSourceRect(std::move(source_rect)); }

        bool isFlipped() const { return image_.isFlipped(); }
        void setFlipped(bool flipped) { image_.setFlipped(flipped); }
    };
}