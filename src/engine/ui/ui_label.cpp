#include "ui_label.h"
#include "../core/context.h"
#include "../render/text_renderer.h"
#include <spdlog/spdlog.h>
#include <entt/core/hashed_string.hpp>
engine::ui::UILabel::UILabel(engine::render::TextRenderer &text_renderer, const std::string &text, const std::string &font_path, int font_size, engine::utils::FColor text_color, glm::vec2 position)
    : UIElement(std::move(position)),
      text_renderer_(text_renderer),
      text_(text),
      font_path_(font_path),
      font_id_(entt::hashed_string(font_path.data())),
      font_size_(font_size),
      text_fcolor_(std::move(text_color))
{
    size_ = text_renderer_.getTextSize(text_, font_id_, font_size_, font_path_);
    spdlog::trace("UILabel size: {} x {}", size_.x, size_.y);
}

void engine::ui::UILabel::render(engine::core::Context &context)
{
    if (!visible_ || text_.empty())
        return;

    text_renderer_.drawUIText(text_, font_id_, font_size_, getScreenPosition(), font_path_, text_fcolor_);

    UIElement::render(context);
}

void engine::ui::UILabel::setText(const std::string &text)
{
    text_ = text;
    size_ = text_renderer_.getTextSize(text_, font_id_, font_size_, font_path_);
}

void engine::ui::UILabel::setFontPath(const std::string &font_path)
{
    font_path_ = font_path;
    font_id_ = entt::hashed_string(font_path.data());
    size_ = text_renderer_.getTextSize(text_, font_id_, font_size_, font_path_);
}

void engine::ui::UILabel::setFontSize(int font_size)
{
    font_size_ = font_size;
    size_ = text_renderer_.getTextSize(text_, font_id_, font_size_, font_path_);
}

void engine::ui::UILabel::setTextFColor(engine::utils::FColor text_fcolor)
{
    text_fcolor_ = std::move(text_fcolor);
}
