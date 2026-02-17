#include "game_scene.h"

// third-party
#include <entt/core/hashed_string.hpp>
#include <entt/signal/dispatcher.hpp>
#include <spdlog/spdlog.h>

// engine - core
#include "../../engine/core/context.h"
#include "../../engine/input/input_manager.h"
#include "../../engine/utils/events.h"
#include "../../engine/audio/audio_player.h"
#include "../../engine/resource/resource_manager.h"
#include "../../engine/loader/level_loader.h"
#include "../../engine/core/game_state.h"

// engine - render & ui
#include "../../engine/render/text_renderer.h"
#include "../../engine/render/camera.h"
#include "../../engine/ui/ui_manager.h"

// engine - component
#include "../../engine/component/transform_component.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/component/velocity_component.h"
#include "../../engine/component/animation_component.h"
#include "../../engine/component/render_component.h"

// engine - system
#include "../../engine/system/render_system.h"
#include "../../engine/system/movement_system.h"
#include "../../engine/system/animation_system.h"
#include "../../engine/system/ysort_system.h"
#include "../../engine/system/audio_system.h"

// game - component & defs
#include "../component/enemy_component.h"
#include "../component/player_component.h"
#include "../component/stats_component.h"
#include "../defs/tags.h"
#include "../spawner/enemy_spawner.h"

// game - system
#include "../system/followpath_system.h"
#include "../system/remove_dead_system.h"
#include "../system/block_system.h"
#include "../system/set_target_system.h"
#include "../system/attack_starter_system.h"
#include "../system/timer_system.h"
#include "../system/orientation_system.h"
#include "../system/animation_state_system.h"
#include "../system/animation_event_system.h"
#include "../system/combat_resolve_system.h"
#include "../system/projectile_system.h"
#include "../system/effect_system.h"
#include "../system/health_bar_system.h"
#include "../system/game_rule_system.h"
#include "../ui/units_portrait_ui.h"
#include "../system/place_unit_system.h"
#include "../system/render_range_system.h"
#include "../system/debug_ui_system.h"
#include "../system/selection_system.h"

// game - loader & factory
#include "../loader/entity_builder_mw.h"
#include "../factory/entity_factory.h"
#include "../factory/blueprint_manager.h"

using namespace entt::literals;

game::scene::GameScene::GameScene(engine::core::Context &context)
    : Scene("GameScene", context)
{
}

game::scene::GameScene::~GameScene()
{
}

void game::scene::GameScene::init()
{
    if (!initSessionData())
    {
        spdlog::error("Failed to init session data");
        return;
    }
    if (!initLevelConfig())
    {
        spdlog::error("Failed to init level config");
        return;
    }
    if (!initUIConfig())
    {
        spdlog::error("Failed to init UI config");
        return;
    }
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
    if (!initInputConnections())
    {
        spdlog::error("Failed to init input connections");
        return;
    }
    if (!initEntityFactory())
    {
        spdlog::error("Failed to init entity factory");
        return;
    }
    if (!initRegistryContext())
    {
        spdlog::error("Failed to init registry context");
        return;
    }
    if (!initUnitsPortraitUI())
    {
        spdlog::error("Failed to init units portrait UI");
        return;
    }
    if (!initSystems())
    {
        spdlog::error("Failed to init systems");
        return;
    }
    if (!initEnemySpawner())
    {
        spdlog::error("Failed to init enemy spawner");
        return;
    }
    Scene::init();
}

void game::scene::GameScene::update(float dt)
{
    auto &dispatcher = context_.getDispatcher();

    // 每一帧最先清理死亡实体(要在dispatcher处理完事件后再清理，因此放在下一帧开头)
    remove_dead_system_->update(registry_);

    timer_system_->update(registry_, dt);
    game_rule_system_->update(dt);
    block_system_->update(registry_, dispatcher);
    set_target_system_->update(registry_);

    follow_path_system_->update(registry_, dispatcher, waypoint_nodes_);
    orientation_system_->update(registry_);
    attack_starter_system_->update(registry_, dispatcher);
    projectile_system_->update(dt);
    movement_system_->update(registry_, dt);
    animation_system_->update(dt);
    place_unit_system_->update(dt);
    ysort_system_->update(registry_);
    selection_system_->update();
    enemy_spawner_->update(dt);
    units_portrait_ui_->update(dt);
    Scene::update(dt);
}

void game::scene::GameScene::render()
{

    auto &renderer = context_.getRender();
    auto &camera = context_.getCamera();

    render_system_->update(registry_, renderer, camera);
    health_bar_system_->update(registry_, renderer, camera);
    render_range_system_->update(registry_, renderer, camera);
    Scene::render();
    debug_ui_system_->update();
}

void game::scene::GameScene::clean()
{
    auto &dispatcher = context_.getDispatcher();
    auto &input_manager = context_.getInputManager();
    dispatcher.disconnect(this);

    input_manager.onAction("pause"_hs).disconnect<&GameScene::onClearAllPlayers>(this);

    Scene::clean();
}

bool game::scene::GameScene::initSessionData()
{
    if (!session_data_)
    {
        session_data_ = std::make_shared<game::data::SessionData>();
        if (!session_data_->loadDefaultData())
        {
            spdlog::error("Failed to load session data");
            return false;
        }
    }
    level_number_ = session_data_->getLevelNumber();
    return true;
}

bool game::scene::GameScene::initLevelConfig()
{
    if (!level_config_)
    {
        level_config_ = std::make_shared<game::data::LevelConfig>();
        if (!level_config_->loadFromFile("assets/data/level_config.json"))
        {
            spdlog::error("Failed to load level config");
            return false;
        }
    }
    waves_ = level_config_->getWavesData(level_number_);
    game_stats_.enemy_count_ = level_config_->getTotalEnemyCount(level_number_);
    return true;
}

bool game::scene::GameScene::initUIConfig()
{
    if (!ui_config_)
    {
        ui_config_ = std::make_shared<game::data::UIConfig>();
        if (!ui_config_->loadFromFile("assets/data/ui_config.json"))
        {
            spdlog::error("Failed to load UI config");
            return false;
        }
    }
    return true;
}

bool game::scene::GameScene::loadlevel()
{
    engine::loader::LevelLoader level_loader;

    level_loader.setEntityBuilder(std::make_unique<game::loader::EntityBuilderMW>(level_loader, context_, registry_, waypoint_nodes_, start_points_));
    // 获取关卡地图路径
    auto map_path = level_config_->getMapPath(level_number_);
    if (!level_loader.loadLevel(map_path, this))
    {
        spdlog::error("Failed to load level");
        return false;
    }

    return true;
}

bool game::scene::GameScene::initEventConnections()
{
    // auto &dispatcher = context_.getDispatcher();

    return true;
}

bool game::scene::GameScene::initInputConnections()
{
    auto &input_manager = context_.getInputManager();
    input_manager.onAction("pause"_hs).connect<&GameScene::onClearAllPlayers>(this);
    return true;
}

bool game::scene::GameScene::initEntityFactory()
{
    // 如果蓝图管理器为空，则创建一个（将来可能由构造函数传入）
    if (!blueprint_manager_)
    {
        blueprint_manager_ = std::make_shared<game::factory::BlueprintManager>(context_.getResourceManager());
        if (!blueprint_manager_->loadEnemyClassBlueprints("assets/data/enemy_data.json") ||
            !blueprint_manager_->loadPlayerClassBlueprints("assets/data/player_data.json") ||
            !blueprint_manager_->loadProjectileBlueprints("assets/data/projectile_data.json") ||
            !blueprint_manager_->loadEffectBlueprints("assets/data/effect_data.json"))
        {

            spdlog::error("Failed to load enemy class blueprints");
            return false;
        }
    }
    entity_factory_ = std::make_unique<game::factory::EntityFactory>(registry_, *blueprint_manager_);
    spdlog::info("entity_factory_ created");
    return true;
}

bool game::scene::GameScene::initRegistryContext()
{
    // 让注册表存储一些数据类型实例作为上下文，方便使用
    registry_.ctx().emplace<std::shared_ptr<game::factory::BlueprintManager>>(blueprint_manager_);
    registry_.ctx().emplace<std::shared_ptr<game::data::SessionData>>(session_data_);
    registry_.ctx().emplace<std::shared_ptr<game::data::UIConfig>>(ui_config_);
    registry_.ctx().emplace<std::shared_ptr<game::data::LevelConfig>>(level_config_);
    registry_.ctx().emplace<std::unordered_map<int, game::data::WaypointNode> &>(waypoint_nodes_);
    registry_.ctx().emplace<std::vector<int> &>(start_points_);
    registry_.ctx().emplace<game::data::GameStats &>(game_stats_);
    registry_.ctx().emplace<game::data::Waves &>(waves_);
    registry_.ctx().emplace<int &>(level_number_);
    registry_.ctx().emplace_as<entt::entity &>("selected_unit"_hs, selected_unit_);
    registry_.ctx().emplace_as<entt::entity &>("hovered_unit"_hs, hovered_unit_);
    spdlog::info("registry_ context initialized");
    return true;
}

bool game::scene::GameScene::initUnitsPortraitUI()
{
    try
    {
        units_portrait_ui_ = std::make_unique<game::ui::UnitsPortraitUI>(registry_, *ui_manager_, context_);
    }
    catch (const std::exception &e)
    {
        spdlog::error("Failed to create units portrait UI: {}", e.what());
        return false;
    }
    return true;
}

bool game::scene::GameScene::initSystems()
{
    auto &dispatcher = context_.getDispatcher();
    // 系统初始化需要在可能的依赖模块(如实体工厂)初始化之后
    render_system_ = std::make_unique<engine::system::RenderSystem>();
    movement_system_ = std::make_unique<engine::system::MovementSystem>();
    animation_system_ = std::make_unique<engine::system::AnimationSystem>(registry_, dispatcher);
    ysort_system_ = std::make_unique<engine::system::YSortSystem>();
    audio_system_ = std::make_unique<engine::system::AudioSystem>(registry_, context_);

    follow_path_system_ = std::make_unique<game::system::FollowPathSystem>();
    remove_dead_system_ = std::make_unique<game::system::RemoveDeadSystem>();
    block_system_ = std::make_unique<game::system::BlockSystem>();
    set_target_system_ = std::make_unique<game::system::SetTargetSystem>();
    attack_starter_system_ = std::make_unique<game::system::AttackStarterSystem>();
    timer_system_ = std::make_unique<game::system::TimerSystem>();
    orientation_system_ = std::make_unique<game::system::OrientationSystem>();
    animation_state_system_ = std::make_unique<game::system::AnimationStateSystem>(registry_, dispatcher);
    animation_event_system_ = std::make_unique<game::system::AnimationEventSystem>(registry_, dispatcher);
    combat_resolve_system_ = std::make_unique<game::system::CombatResolveSystem>(registry_, dispatcher);
    projectile_system_ = std::make_unique<game::system::ProjectileSystem>(registry_, dispatcher, *entity_factory_);
    effect_system_ = std::make_unique<game::system::EffectSystem>(registry_, dispatcher, *entity_factory_);
    health_bar_system_ = std::make_unique<game::system::HealthBarSystem>();
    game_rule_system_ = std::make_unique<game::system::GameRuleSystem>(registry_, dispatcher);
    place_unit_system_ = std::make_unique<game::system::PlaceUnitSystem>(registry_, *entity_factory_, context_);
    render_range_system_ = std::make_unique<game::system::RenderRangeSystem>();
    debug_ui_system_ = std::make_unique<game::system::DebugUISystem>(registry_, context_);
    selection_system_ = std::make_unique<game::system::SelectionSystem>(registry_, context_);
    spdlog::info("Systems initialized");
    return true;
}

bool game::scene::GameScene::initEnemySpawner()
{
    enemy_spawner_ = std::make_unique<game::spawner::EnemySpawner>(registry_, *entity_factory_);
    spdlog::info("Enemy spawner initialized");
    return true;
}

bool game::scene::GameScene::onClearAllPlayers()
{
    auto view = registry_.view<game::component::PlayerComponent>();
    for (auto entity : view)
    {
        context_.getDispatcher().enqueue(game::defs::RemovePlayerUnitEvent{entity});
    }
    return true;
}
