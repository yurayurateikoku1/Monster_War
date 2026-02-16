#include "ui_image.h"
#include "../render/render.h"
#include "../render/sprite.h"
#include "../core/context.h"
#include <spdlog/spdlog.h>

engine::ui::UIImage::UIImage(const std::string &texture_path, const glm::vec2 &position, const glm::vec2 &size, const std::optional<engine::utils::Rect> &source_rect, bool is_flipped)
    : UIElement(position, size), sprite_(texture_path, source_rect, is_flipped)
{
    if (sprite_.getTextureId() == entt::null)
    {
        spdlog::error("texture_id is empty");
    }
}

engine::ui::UIImage::UIImage(entt::id_type texture_id, glm::vec2 position, glm::vec2 size, std::optional<engine::utils::Rect> source_rect, bool is_flipped)
    : UIElement(position, size), sprite_(texture_id, source_rect, is_flipped)
{
    if (sprite_.getTextureId() == entt::null)
    {
        spdlog::error("texture_id is empty");
    }
}

engine::ui::UIImage::UIImage(engine::render::Sprite &image, glm::vec2 position, glm::vec2 size)
    : UIElement(position, size), sprite_(image)
{
    if (sprite_.getTextureId() == entt::null)
    {
        spdlog::error("texture_id is empty");
    }
}

void engine::ui::UIImage::render(engine::core::Context &context)
{
    if (!visible_ || sprite_.getTextureId() == entt::null)
    {
        return;
    }

    auto position = getScreenPosition();
    if (size_.x == 0.0f && size_.y == 0.0f)
    {
        context.getRender().drawUISprite(sprite_, position);
    }
    else
    {
        context.getRender().drawUISprite(sprite_, position, size_);
    }
    UIElement::render(context);
}
