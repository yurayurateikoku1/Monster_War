#include "sprite_component.h"
#include "transform_component.h"
#include <spdlog/spdlog.h>
#include "../resource/resource_manager.h"
#include "../render/render.h"
#include "../render/camera.h"
#include "../core/context.h"
#include "../object/game_object.h"
engine::component::SpriteComponent::SpriteComponent(const std::string &texture_id, engine::resource::ResourceManager &resource_manager, engine::utils::Alignment alignment, std::optional<SDL_Rect> source_rect_opt, bool is_flipped)
    : resourceManager_(&resource_manager), alignment_(alignment), sprite_(texture_id, source_rect_opt, is_flipped)
{
    if (!resourceManager_)
    {
        spdlog::error("ResourceManager is not initialized.");
    }
}

engine::component::SpriteComponent::SpriteComponent(engine::render::Sprite &&sprite, engine::resource::ResourceManager &resource_manager, engine::utils::Alignment alignment)
    : resourceManager_(&resource_manager), alignment_(alignment), sprite_(std::move(sprite))
{
    if (!resourceManager_)
    {
        spdlog::error("ResourceManager is not initialized.");
    }
}

void engine::component::SpriteComponent::updateOffset()
{
    if (sprite_size_.x <= 0 || sprite_size_.y <= 0)
    {
        /* code */
        offset_ = {0.0f, 0.0f};
        return;
    }
    auto scale = transform_->getScale();
    switch (alignment_)
    {
    case engine::utils::Alignment::TOP_LEFT:
        offset_ = glm::vec2{0.0f, 0.0f} * scale;
        break;
    case engine::utils::Alignment::TOP_CENTER:
        offset_ = glm::vec2{-sprite_size_.x / 2.0f, 0.0f} * scale;
        break;
    case engine::utils::Alignment::TOP_RIGHT:
        offset_ = glm::vec2{-sprite_size_.x, 0.0f} * scale;
        break;
    case engine::utils::Alignment::CENTER_LEFT:
        offset_ = glm::vec2{0.0f, -sprite_size_.y / 2.0f} * scale;
        break;
    case engine::utils::Alignment::CENTER:
        offset_ = glm::vec2{-sprite_size_.x / 2.0f, -sprite_size_.y / 2.0f} * scale;
        break;
    case engine::utils::Alignment::CENTER_RIGHT:
        offset_ = glm::vec2{-sprite_size_.x, -sprite_size_.y / 2.0f} * scale;
        break;
    case engine::utils::Alignment::BOTTOM_LEFT:
        offset_ = glm::vec2{0.0f, -sprite_size_.y} * scale;
        break;
    case engine::utils::Alignment::BOTTOM_CENTER:
        offset_ = glm::vec2{-sprite_size_.x / 2.0f, -sprite_size_.y} * scale;
        break;
    case engine::utils::Alignment::BOTTOM_RIGHT:
        offset_ = glm::vec2{-sprite_size_.x, -sprite_size_.y} * scale;
        break;
    default:
        break;
    }
}

void engine::component::SpriteComponent::setSpriteById(const std::string &texture_id, const std::optional<SDL_Rect> &source_rect)
{
    sprite_.setTextureId(texture_id);
    sprite_.setSourceRect(source_rect);
    updateSpriteSize();
    updateOffset();
}

void engine::component::SpriteComponent::setSourceRect(const std::optional<SDL_Rect> &source_rect_opt)
{
    sprite_.setSourceRect(source_rect_opt);
    updateSpriteSize();
    updateOffset();
}

void engine::component::SpriteComponent::setAlignment(engine::utils::Alignment anchor)
{
    alignment_ = anchor;
    updateOffset();
}

void engine::component::SpriteComponent::updateSpriteSize()
{
    if (!resourceManager_)
    {
        spdlog::error("ResourceManager is not initialized.");
        return;
    }
    if (sprite_.getSourceRect().has_value())
    {
        const auto &src_rect = sprite_.getSourceRect().value();
        sprite_size_ = {src_rect.w, src_rect.h};
    }
    else
    {
        sprite_size_ = resourceManager_->getTextureSize(sprite_.getTextureId());
    }
}

void engine::component::SpriteComponent::init()
{
    if (!owner_)
    {
        spdlog::error("SpriteComponent: Owner is not set.");
        return;
    }
    transform_ = owner_->getComponent<engine::component::TransformComponent>();
    if (!transform_)
    {
        spdlog::warn("GameObject {} does not have a TransformComponent.", owner_->getName());
        return;
    }
    updateSpriteSize();
    updateOffset();
}

void engine::component::SpriteComponent::render(engine::core::Context &context)
{
    if (is_hidden_ || !transform_ || !resourceManager_)
    {
        return;
    }

    const glm::vec2 &position = transform_->getPosition() + offset_;
    const glm::vec2 &scale = transform_->getScale();
    const float rotation = transform_->getRotation();
    context.getRender().drawSprite(context.getCamera(), sprite_, position, scale, rotation);
}
