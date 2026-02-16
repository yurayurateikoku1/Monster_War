#include "parallax_component.h"
#include "transform_component.h"
#include <spdlog/spdlog.h>
#include "../core/context.h"
#include "../object/game_object.h"
#include "../render/render.h"
#include "../render/camera.h"
#include "../render/sprite.h"
engine::component::ParallaxComponent::ParallaxComponent(const std::string &texture_id, const glm::vec2 &scroll_factor, const glm::bvec2 &repeat)
    : sprite_(engine::render::Sprite(texture_id)), scroll_factor_(scroll_factor), repeat_(repeat)
{
    spdlog::info("ParallaxComponent created");
}

void engine::component::ParallaxComponent::init()
{
    if (!owner_)
    {
        /* code */
        spdlog::error("ParallaxComponent: Owner is not set.");
        return;
    }
    transform_ = owner_->getComponent<engine::component::TransformComponent>();
    if (!transform_)
    {
        /* code */
        spdlog::error("GameObject {} does not have a TransformComponent.", owner_->getName());
        return;
    }
}

void engine::component::ParallaxComponent::render(engine::core::Context &context)
{
    if (is_hidden_ || !transform_)
    {
        /* code */
        return;
    }
    context.getRender().drawParallx(context.getCamera(), sprite_, transform_->getPosition(), scroll_factor_, repeat_);
}
