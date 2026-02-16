#pragma once
#include <string>
#include <unordered_map>
#include <array>
#include <vector>
#include <SDL3/SDL_render.h>
#include <glm/vec2.hpp>
#include <variant>
#include <entt/signal/sigh.hpp>
#include <entt/signal/fwd.hpp>
namespace engine::core
{
    class Config;
}

namespace engine::input
{
    enum class ActionState
    {
        PRESSED,  // 刚刚按下
        HELD,     // 持续按压
        RELEASED, // 刚刚释放
        INACTIVE  // 未触发

    };

    /// @brief 输入管理器
    class InputManager final
    {
    private:
        /* data */
        SDL_Renderer *sdl_renderer_{nullptr};
        entt::dispatcher *dispatcher_;
        /// @brief 动作到函数的映射 每个动作包含三个状态 RELEASED, PRESSED, HELD，每个状态对应一个函数
        std::unordered_map<std::string, std::array<entt::sigh<bool()>, 3>> action2func_;

        /// @brief 存储每个动作的当前状态
        std::unordered_map<std::string, ActionState> action_states_;

        /// @brief 按键到动作的映射
        std::unordered_map<std::variant<SDL_Scancode, Uint32>, std::vector<std::string>> input2action_;

        /// @brief 鼠标位置
        glm::vec2 mouse_position_{0.0f, 0.0f};
        glm::vec2 logical_mouse_position_;

    public:
        InputManager(SDL_Renderer *sdl_renderer, const engine::core::Config *config, entt::dispatcher *dispatcher);

        /// @brief 注册一个动作的回调
        /// @param action_name
        /// @param state
        /// @return 一个sink对象，用于注册回调
        entt::sink<entt::sigh<bool()>> onAction(const std::string &action_name, ActionState state = ActionState::PRESSED);

        void update();
        void quit();

        /// @brief 动作当前是否触发,持续按压
        /// @param action_name
        /// @return
        bool isActionDown(const std::string &action_name) const;

        /// @brief 动作当前是否在本帧刚刚按下
        /// @param action_name
        /// @return
        bool isActionPressed(const std::string &action_name) const;

        /// @brief 动作当前是否在本帧刚刚释放
        /// @param action_name
        /// @return
        bool isActionReleased(const std::string &action_name) const;

        glm::vec2 getMousePosition() const { return mouse_position_; }
        glm::vec2 getLogicalMousePosition() const;

    private:
        /// @brief 处理事件，将按键状态转换为动作状态
        /// @param event
        void processEvent(const SDL_Event &event);
        void initMappings(const engine::core::Config *config);
        void updateActionStates(const std::string &action_name, bool is_input_active, bool is_reapt_event);
        SDL_Scancode scancodeFromString(const std::string &key_name);
        Uint32 mouseButtonFromString(const std::string &button_name);
    };

}
