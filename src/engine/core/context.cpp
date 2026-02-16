#include "context.h"
#include <spdlog/spdlog.h>
engine::core::Context::Context(entt::dispatcher &dispatcher, engine::input::InputManager &input_manager, engine::render::Renderer &render, engine::resource::ResourceManager &resource_manager, engine::render::Camera &camera, engine::render::TextRenderer &text_renderer, engine::audio::AudioPlayer &audio_player, engine::core::GameState &game_state)
    : dispatcher_(dispatcher), input_manager_(input_manager), renderer_(render), resource_manager_(resource_manager), camera_(camera), text_renderer_(text_renderer), audio_player_(audio_player), game_state_(game_state)
{
    spdlog::info("Context created");
}
