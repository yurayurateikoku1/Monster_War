#include "game_scene.h"
#include "../../engine/core/context.h"
#include "../../engine/input/input_manager.h"
#include "../../engine/utils/events.h"
#include <entt/signal/dispatcher.hpp>
#include <spdlog/spdlog.h>
#include "../../engine/component/transform_component.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/component/velocity_component.h"
#include "../../engine/component/animation_component.h"
#include "../../engine/system/render_system.h"
#include "../../engine/system/movement_system.h"
#include "../../engine/system/animation_system.h"
#include "../../engine/system/ysort_system.h"
#include "../../engine/audio/audio_player.h"
#include "../../engine/resource/resource_manager.h"
#include "../../engine/render/text_renderer.h"
#include "../../engine/ui/ui_manager.h"
#include "../../engine/ui/ui_image.h"
#include "../../engine/ui/ui_label.h"
#include "../../engine/loader/level_loader.h"
#include <entt/core/hashed_string.hpp>

#include "../loader/entity_builder_mw.h"
#include "../component/enemy_component.h"
#include "../loader/entity_builder_mw.h"
#include "../system/followpath_system.h"
#include "../system/remove_dead_system.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/component/velocity_component.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/component/render_component.h"

#include "../factory/entity_factory.h"
#include "../factory/blueprint_manager.h"
using namespace entt::literals;

game::scene::GameScene::GameScene(engine::core::Context &context)
    : Scene("GameScene", context)
{
    render_system_ = std::make_unique<engine::system::RenderSystem>();
    movement_system_ = std::make_unique<engine::system::MovementSystem>();
    animation_system_ = std::make_unique<engine::system::AnimationSystem>();
    ysort_system_ = std::make_unique<engine::system::YSortSystem>();

    follow_path_system_ = std::make_unique<game::system::FollowPathSystem>();
    remove_dead_system_ = std::make_unique<game::system::RemoveDeadSystem>();
}

game::scene::GameScene::~GameScene()
{
}

void game::scene::GameScene::init()
{
    if (!loadlevel())
    {
        spdlog::error("Failed to load level");
        return;
    }
    if (!initEventConnections())
    {
        spdlog::error("Failed to init event connections");
        return;
    }

    if (!initEntityFactory())
    {
        spdlog::error("Failed to init entity factory");
        return;
    }
    createTestEnemy();
    Scene::init();
}

void game::scene::GameScene::update(float dt)
{
    auto &dispatcher = context_.getDispatcher();

    // 每一帧最先清理死亡实体(要在dispatcher处理完事件后再清理，因此放在下一帧开头)
    remove_dead_system_->update(registry_);

    // 注意系统更新的顺序
    follow_path_system_->update(registry_, dispatcher, waypoint_nodes_);
    movement_system_->update(registry_, dt);
    animation_system_->update(registry_, dt);
    ysort_system_->update(registry_);
    Scene::update(dt);
}

void game::scene::GameScene::render()
{
    render_system_->update(registry_, context_.getRender(), context_.getCamera());

    Scene::render();
}

void game::scene::GameScene::clean()
{
    auto &dispatcher = context_.getDispatcher();
    dispatcher.disconnect(this);
    Scene::clean();
}

bool game::scene::GameScene::loadlevel()
{
    engine::loader::LevelLoader level_loader;

    level_loader.setEntityBuilder(std::make_unique<game::loader::EntityBuilderMW>(level_loader, context_, registry_, waypoint_nodes_, start_points_));
    if (!level_loader.loadLevel("assets/maps/level1.tmj", this))
    {
        spdlog::error("Failed to load level");
        return false;
    }

    return true;
}

bool game::scene::GameScene::initEventConnections()
{
    auto &dispatcher = context_.getDispatcher();
    // 断开所有事件连接
    dispatcher.sink<game::defs::EnemyArriveHomeEvent>().connect<&GameScene::onEnemyArriveHome>(this);
    return true;
}

bool game::scene::GameScene::initEntityFactory()
{
    // 如果蓝图管理器为空，则创建一个（将来可能由构造函数传入）
    if (!blueprint_manager_)
    {
        blueprint_manager_ = std::make_shared<game::factory::BlueprintManager>(context_.getResourceManager());
        if (!blueprint_manager_->loadEnemyClassBlueprints("assets/data/enemy_data.json"))
        {
            spdlog::error("Failed to load enemy class blueprints");
            return false;
        }
    }
    entity_factory_ = std::make_unique<game::factory::EntityFactory>(registry_, *blueprint_manager_);
    spdlog::info("entity_factory_ created");
    return true;
}

void game::scene::GameScene::onEnemyArriveHome(const game::defs::EnemyArriveHomeEvent &event)
{
    spdlog::info("Enemy arrive home");
}

void game::scene::GameScene::createTestEnemy()
{
    // 每个起点创建一个敌人
    for (auto start_index : start_points_)
    {
        auto position = waypoint_nodes_[start_index].position_;
        entity_factory_->createEnemyUnit("wolf"_hs, position, start_index);
        entity_factory_->createEnemyUnit("slime"_hs, position, start_index);
        entity_factory_->createEnemyUnit("goblin"_hs, position, start_index);
        entity_factory_->createEnemyUnit("dark_witch"_hs, position, start_index);
    }
}
