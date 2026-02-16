#pragma once
#include "ui_element.h"
#include "../utils/math.h"
#include "../render/text_renderer.h"
#include <string>
#include <entt/entity/fwd.hpp>
namespace engine::ui
{
    class UILabel final : public UIElement
    {
    private:
        engine::render::TextRenderer &text_renderer_;
        std::string text_;
        std::string font_path_;
        entt::id_type font_id_;
        int font_size_;
        engine::utils::FColor text_fcolor_ = {1.0f, 1.0f, 1.0f, 1.0f};

    public:
        UILabel(engine::render::TextRenderer &text_renderer,
                const std::string &text,
                const std::string &font_path,
                int font_size = 16,
                engine::utils::FColor text_color = {1.0f, 1.0f, 1.0f, 1.0f},
                glm::vec2 position = {0.0f, 0.0f});

        void render(engine::core::Context &context) override;

        std::string getText() const { return text_; }
        entt::id_type getFontId() const { return font_id_; }
        int getFontSize() const { return font_size_; }
        const engine::utils::FColor &getTextFColor() const { return text_fcolor_; }

        void setText(const std::string &text);          ///< @brief 设置文本内容, 同时更新尺寸
        void setFontPath(const std::string &font_path); ///< @brief 设置字体ID, 同时更新尺寸
        void setFontSize(int font_size);                ///< @brief 设置字体大小, 同时更新尺寸
        void setTextFColor(engine::utils::FColor text_fcolor);
    };
}