#pragma once
#include "../data/waypoint_node.h"
#include <entt/entity/fwd.hpp>
#include <entt/signal/fwd.hpp>
#include <unordered_map>

namespace game::system
{
    class FollowPathSystem
    {
    public:
        void update(entt::registry &registry, entt::dispatcher &dispatcher, std::unordered_map<int, data::WaypointNode> &waypoint_nodes);
    };
}