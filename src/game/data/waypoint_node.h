#pragma once
#include <glm/vec2.hpp>
#include <vector>

namespace game::data
{
    /// @brief 路径节点
    struct WaypointNode
    {
        int id_;
        glm::vec2 position_;
        std::vector<int> next_node_ids_;
    };
}
