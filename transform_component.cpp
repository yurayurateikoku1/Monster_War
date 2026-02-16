#include "transform_component.h"
#include <spdlog/spdlog.h>
#include "../object/game_object.h"
#include "sprite_component.h"

void engine::component::TransformComponent::setScale(const glm::vec2 &scale)
{
    scale_ = scale;
    if (owner_)
    {
        auto sprite_comp = owner_->getComponent<engine::component::SpriteComponent>();
        if (sprite_comp)
        {
            /* code */
            sprite_comp->updateOffset(); // 更新偏移
        }
    }
}
