#include "ysort_system.h"
#include "../component/render_component.h"
#include "../component/transform_component.h"
#include <entt/entity/registry.hpp>
void engine::system::YSortSystem::update(entt::registry &registry)
{
    auto view = registry.view<engine::component::RenderComponent, const engine::component::TransformComponent>();
    for (auto entity : view)
    {
        auto &render = view.get<engine::component::RenderComponent>(entity);
        const auto &transform = view.get<engine::component::TransformComponent>(entity);
        render.depth_ = transform.position_.y;
    }
}