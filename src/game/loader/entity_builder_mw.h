#pragma once
#include "../../engine/loader/basic_entity_builder.h"
#include "../data/waypoint_node.h"

namespace game::loader
{
    class EntityBuilderMW : public engine::loader::BasicEntityBuilder
    {
    private:
        std::unordered_map<int, data::WaypointNode> &waypoint_nodes_;
        std::vector<int> &start_points_;

    public:
        EntityBuilderMW(engine::loader::LevelLoader &level_loader, engine::core::Context &context, entt::registry &registry,
                        std::unordered_map<int, data::WaypointNode> &waypoint_nodes, std::vector<int> &start_points);
        ~EntityBuilderMW() = default;

        EntityBuilderMW *build() override;

    private:
        void buildPath();
        void buildPlace(); ///< @brief 生成单位放置区域标签
    };
}