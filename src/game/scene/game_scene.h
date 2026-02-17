#pragma once
#include "../../engine/scene/scene.h"
#include "../../engine/system/fwd.h"
#include "../data/waypoint_node.h"
#include "../defs/events.h"
#include "../system/fwd.h"
#include <memory>
#include <unordered_map>
#include <vector>
namespace game::scene
{
    class GameScene final : public engine::scene::Scene
    {
    private:
        std::unique_ptr<engine::system::RenderSystem> render_system_;
        std::unique_ptr<engine::system::MovementSystem> movement_system_;
        std::unique_ptr<engine::system::AnimationSystem> animation_system_;
        std::unique_ptr<engine::system::YSortSystem> ysort_system_;
        std::unique_ptr<game::system::FollowPathSystem> follow_path_system_;
        std::unique_ptr<game::system::RemoveDeadSystem> remove_dead_system_;

        std::unordered_map<int, game::data::WaypointNode> waypoint_nodes_; // <id, 路径节点>
        std::vector<int> start_points_;                                    // 起始点
    public:
        GameScene(engine::core::Context &context);
        ~GameScene();

        void init() override;
        void update(float dt) override;
        void render() override;
        void clean() override;

    private:
        [[nodiscard]] bool loadlevel();
        [[nodiscard]] bool initEventConnections();

        // 事件回调函数
        void onEnemyArriveHome(const game::defs::EnemyArriveHomeEvent &event);

        // 测试函数
        void createTestEnemy();
    };
}