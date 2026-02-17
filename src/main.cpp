#include "engine/core/game_app.h"
#include "engine/core/context.h"
#include "engine/scene/scene_manager.h"
#include "game/scene/splash_scene.h"
#include <spdlog/spdlog.h>
#include <SDL3/SDL_main.h>
#include <entt/signal/dispatcher.hpp>
#include "engine/utils/events.h"
void setupInitialScene(engine::core::Context &context)
{
    auto splash_scene = std::make_unique<game::scene::SplashScene>(context);
    context.getDispatcher().trigger<engine::utils::PushSceneEvent>(engine::utils::PushSceneEvent{std::move(splash_scene)});
}

int main(int, char *[])
{
    engine::core::GameApp app;

    app.registerSceneSutep(setupInitialScene);
    app.run();
    return 0;
}
