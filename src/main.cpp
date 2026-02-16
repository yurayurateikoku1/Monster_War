#include "engine/core/game_app.h"
#include "engine/core/context.h"
#include "engine/scene/scene_manager.h"
#include "game/scene/game_scene.h"
#include <spdlog/spdlog.h>
#include <SDL3/SDL_main.h>
#include <entt/signal/dispatcher.hpp>
#include "engine/utils/events.h"
void setupInitialScene(engine::core::Context &context)
{
    auto game_scene = std::make_unique<game::scene::GameScene>(context);
    context.getDispatcher().trigger<engine::utils::PushSceneEvent>(engine::utils::PushSceneEvent{std::move(game_scene)});
}

int main(int, char *[])
{
    engine::core::GameApp app;

    app.registerSceneSutep(setupInitialScene);
    app.run();
    return 0;
}
