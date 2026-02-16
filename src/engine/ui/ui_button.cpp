#include "ui_button.h"
#include "state/ui_normal_state.h"
#include <spdlog/spdlog.h>
#include <entt/core/hashed_string.hpp>
using namespace entt::literals;

engine::ui::UIButton::UIButton(engine::core::Context &context, const std::string &norml_sprite_id, const std::string &hover_sprite_id, const std::string &pressed_sprite_id, const glm::vec2 &position, const glm::vec2 &size, std::function<void()> callback)
    : UIInteractive(context, position, size), callback_(std::move(callback))
{
    addSprite("normal"_hs, engine::render::Sprite(norml_sprite_id));
    addSprite("hover"_hs, engine::render::Sprite(hover_sprite_id));
    addSprite("pressed"_hs, engine::render::Sprite(pressed_sprite_id));
    setState(std::make_unique<engine::ui::state::UINormalState>(this));
    addSound("hover"_hs, "assets/audio/button_hover.wav"_hs);
    addSound("pressed"_hs, "assets/audio/button_click.wav"_hs);
    spdlog::info("UIButton created");
}

void engine::ui::UIButton::clicked()
{
    if (callback_)
    {
        callback_();
    }
}
