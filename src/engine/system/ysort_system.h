#pragma once
#include <entt/entity/fwd.hpp>

namespace engine::system
{
    class YSortSystem
    {
    public:
        void update(entt::registry &registry);
    };
}