#include "ui_button.h"
#include "state/ui_normal_state.h"
#include <spdlog/spdlog.h>
#include <entt/core/hashed_string.hpp>
using namespace entt::literals;

engine::ui::UIButton::UIButton(engine::core::Context &context, const std::string &norml_image_id, const std::string &hover_image_id, const std::string &pressed_image_id, const glm::vec2 &position, const glm::vec2 &size, std::function<void()> callback)
    : UIInteractive(context, position, size), callback_(std::move(callback))
{
    addImage("normal"_hs, engine::render::Image(norml_image_id));
    addImage("hover"_hs, engine::render::Image(hover_image_id));
    addImage("pressed"_hs, engine::render::Image(pressed_image_id));
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
