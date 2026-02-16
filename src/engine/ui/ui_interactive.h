#pragma once
#include "ui_element.h"
#include "state/ui_state.h"
#include "../render/image.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <entt/core/hashed_string.hpp>
namespace engine::core
{
    class Context;
}

namespace engine::ui
{
    class UIInteractive : public UIElement
    {
    protected:
        engine::core::Context &context_;
        std::unique_ptr<engine::ui::state::UIState> state_;
        std::unordered_map<entt::id_type, engine::render::Image> images_;
        std::unordered_map<entt::id_type, entt::id_type> sounds_;
        entt::id_type current_image_id_;
        bool interactive_ = true;

    public:
        UIInteractive(engine::core::Context &context, glm::vec2 position = {0.0f, 0.0f}, glm::vec2 size = {0.0f, 0.0f});
        ~UIInteractive() override;

        virtual void clicked() {}

        void addImage(entt::id_type name_id, engine::render::Image image); ///< @brief 添加精灵
        void setImage(entt::id_type name_id);                              ///< @brief 设置当前显示的精灵
        void addSound(entt::id_type name_id, entt::hashed_string path);    ///< @brief 添加音效
        void playSound(entt::id_type name_id);                             ///< @brief 播放音效
        // --- Getters and Setters ---
        void setState(std::unique_ptr<engine::ui::state::UIState> state);     ///< @brief 设置当前状态
        engine::ui::state::UIState *getState() const { return state_.get(); } ///< @brief 获取当前状态

        void setInteractive(bool interactive) { interactive_ = interactive; } ///< @brief 设置是否可交互
        bool isInteractive() const { return interactive_; }                   ///< @brief 获取是否可交互
        bool handleInput(engine::core::Context &context) override;
        void render(engine::core::Context &context) override;
    };
}