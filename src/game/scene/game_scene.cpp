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
#include "../../engine/audio/audio_player.h"
#include "../../engine/resource/resource_manager.h"
#include "../../engine/render/text_renderer.h"
#include "../../engine/ui/ui_manager.h"
#include "../../engine/ui/ui_image.h"
#include "../../engine/ui/ui_label.h"
#include "../../engine/loader/level_loader.h"
#include <entt/core/hashed_string.hpp>
using namespace entt::literals;

game::scene::GameScene::GameScene(engine::core::Context &context)
    : Scene("GameScene", context)
{
    render_system_ = std::make_unique<engine::system::RenderSystem>();
    movement_system_ = std::make_unique<engine::system::MovementSystem>();
    animation_system_ = std::make_unique<engine::system::AnimationSystem>();
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

    Scene::init();
}

void game::scene::GameScene::update(float dt)
{
    movement_system_->update(registry_, dt);
    animation_system_->update(registry_, dt);

    Scene::update(dt);
}

void game::scene::GameScene::render()
{
    render_system_->update(registry_, context_.getRender(), context_.getCamera());

    Scene::render();
}

void game::scene::GameScene::clean()
{

    Scene::clean();
}

bool game::scene::GameScene::loadlevel()
{
    engine::loader::LevelLoader level_loader;
    if (!level_loader.loadLevel("assets/maps/level1.tmj", this))
    {
        spdlog::error("Failed to load level");
        return false;
    }

    return true;
}
