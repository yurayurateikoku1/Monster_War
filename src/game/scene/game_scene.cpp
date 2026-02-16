#include "game_scene.h"
#include "../../engine/core/context.h"
#include "../../engine/input/input_manager.h"
#include "../../engine/utils/events.h"
#include <entt/signal/dispatcher.hpp>
#include <spdlog/spdlog.h>

#include "../../engine/audio/audio_player.h"
#include "../../engine/resource/resource_manager.h"
#include "../../engine/render/text_renderer.h"
#include "../../engine/ui/ui_manager.h"
#include "../../engine/ui/ui_image.h"
#include "../../engine/ui/ui_label.h"
#include <entt/core/hashed_string.hpp>
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
    testResourceMana();
    Scene::init();
}

void game::scene::GameScene::clean()
{

    Scene::clean();
}

void game::scene::GameScene::testResourceMana()
{
    // 载入资源
    context_.getResourceManager().loadTexture("assets/textures/Buildings/Castle.png"_hs);
    // 播放音乐
    context_.getAudioPlayer().playMusic("battle_bgm"_hs);

    // 测试UI元素（使用载入的资源）
    ui_manager_->addElement(std::make_unique<engine::ui::UIImage>("assets/textures/Buildings/Castle.png"_hs));
    ui_manager_->addElement(std::make_unique<engine::ui::UILabel>(
        context_.getTextRenderer(),
        "Hello, World!",
        "assets/fonts/VonwaonBitmap-16px.ttf"));
}
