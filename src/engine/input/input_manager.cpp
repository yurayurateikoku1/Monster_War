#include "input_manager.h"
#include <spdlog/spdlog.h>
#include <SDL3/SDL.h>
#include "../core/config.h"
#include <glm/vec2.hpp>
#include <entt/signal/dispatcher.hpp>
#include "../utils/events.h"
engine::input::InputManager::InputManager(SDL_Renderer *sdl_renderer, const engine::core::Config *config, entt::dispatcher *dispatcher)
    : sdl_renderer_(sdl_renderer), dispatcher_(dispatcher)
{
    if (!sdl_renderer_)
    {
        spdlog::error("SDL_Renderer is null");
        throw std::runtime_error("SDL_Renderer is null");
    }
    initMappings(config);
    float x, y;
    SDL_GetMouseState(&x, &y);
    mouse_position_ = glm::vec2(x, y);
}

entt::sink<entt::sigh<bool()>> engine::input::InputManager::onAction(const std::string &action_name, ActionState state)
{
    return action2func_[std::string(action_name)].at(static_cast<size_t>(state));
}

void engine::input::InputManager::update()
{
    // 根据上一帧动作状态更新
    for (auto &[action_name, state] : action_states_)
    {
        if (state == ActionState::PRESSED)
        {
            state = ActionState::HELD;
        }
        else if (state == ActionState::RELEASED)
        {
            state = ActionState::INACTIVE;
        }
    }
    // 处理待处理的SDL事件
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        processEvent(event);
    }
    // 触发回调
    for (auto &[action_name, state] : action_states_)
    {
        if (state != ActionState::INACTIVE)
        {
            if (auto it = action2func_.find(action_name); it != action2func_.end())
            {
                it->second.at(static_cast<size_t>(state)).collect([](bool result)
                                                                  { return result; }); // 触发回调
            }
        }
    }
}

void engine::input::InputManager::quit()
{
    dispatcher_->trigger<engine::utils::QuitEvent>();
}

bool engine::input::InputManager::isActionDown(const std::string &action_name) const
{
    if (auto it = action_states_.find(action_name); it != action_states_.end())
    {
        return it->second == ActionState::HELD || it->second == ActionState::PRESSED;
    }

    return false;
}

bool engine::input::InputManager::isActionPressed(const std::string &action_name) const
{
    if (auto it = action_states_.find(action_name); it != action_states_.end())
    {
        return it->second == ActionState::PRESSED;
    }
    return false;
}

bool engine::input::InputManager::isActionReleased(const std::string &action_name) const
{
    if (auto it = action_states_.find(action_name); it != action_states_.end())
    {
        return it->second == ActionState::RELEASED;
    }
    return false;
}

glm::vec2 engine::input::InputManager::getLogicalMousePosition() const
{
    return logical_mouse_position_;
}

void engine::input::InputManager::processEvent(const SDL_Event &event)
{
    switch (event.type)
    {
    case SDL_EVENT_KEY_DOWN:
    case SDL_EVENT_KEY_UP:
    {
        SDL_Scancode scancode = event.key.scancode;
        bool is_down = event.key.down;
        bool is_repeat = event.key.repeat;
        auto it = input2action_.find(scancode);
        if (it != input2action_.end())
        {
            const std::vector<std::string> &associated_actions = it->second;
            for (const std::string &action_name : associated_actions)
            {
                updateActionStates(action_name, is_down, is_repeat);
            }
        }
        break;
    }
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP:
    {
        Uint8 mouse_button = event.button.button;
        bool is_down_mouse = event.button.down;
        auto it_mouse = input2action_.find(mouse_button);
        if (it_mouse != input2action_.end())
        {
            const std::vector<std::string> &associated_actions = it_mouse->second;
            for (const std::string &action_name : associated_actions)
            {
                updateActionStates(action_name, is_down_mouse, false);
            }
        }
        mouse_position_ = {event.motion.x, event.motion.y};
        SDL_RenderCoordinatesFromWindow(sdl_renderer_, mouse_position_.x, mouse_position_.y, &logical_mouse_position_.x, &logical_mouse_position_.y);
        break;
    }
    case SDL_EVENT_MOUSE_MOTION:
    {
        mouse_position_ = {event.motion.x, event.motion.y};
        SDL_RenderCoordinatesFromWindow(sdl_renderer_, mouse_position_.x, mouse_position_.y, &logical_mouse_position_.x, &logical_mouse_position_.y);

        break;
    }
    case SDL_EVENT_QUIT:
    {
        quit();
        break;
    }
    default:
        break;
    }
}

void engine::input::InputManager::initMappings(const engine::core::Config *config)
{
    if (!config)
    {
        spdlog::error("Config is null");
        throw std::runtime_error("Config is null");
    }
    auto action2keyname_map_ = config->_input_mappings;
    input2action_.clear();
    action_states_.clear();

    // 如果没有映射，添加默认映射
    if (action2keyname_map_.find("mouse_left") == action2keyname_map_.end())
    {
        action2keyname_map_["mouse_left"] = {"MouseLeft"};
    }
    if (action2keyname_map_.find("mouse_right") == action2keyname_map_.end())
    {
        action2keyname_map_["mouse_right"] = {"MouseRight"};
    }

    // 遍历action2keyname_map_
    for (const auto &[action_name, key_names] : action2keyname_map_)
    {
        action_states_[action_name] = ActionState::INACTIVE;
        for (const std::string &key_name : key_names)
        {
            /* code */
            SDL_Scancode scancode = scancodeFromString(key_name);
            Uint32 mouse_button = mouseButtonFromString(key_name);

            if (scancode != SDL_SCANCODE_UNKNOWN)
            {
                /* code */
                input2action_[scancode].push_back(action_name);
                spdlog::info("Mapped key '{}' (scancode: {}) to action '{}'", key_name, static_cast<int>(scancode), action_name);
            }
            else if (mouse_button != 0)
            {
                input2action_[mouse_button].push_back(action_name);
                spdlog::info("Mapped mouse button '{}' to action '{}'", key_name, action_name);
            }
            else
            {
                spdlog::warn("Unknown key name: {}", key_name);
            }
        }
    }
}

void engine::input::InputManager::updateActionStates(const std::string &action_name, bool is_input_active, bool is_reapt_event)
{
    auto it = action_states_.find(action_name);
    if (it == action_states_.end())
    {
        return;
    }
    if (is_input_active)
    {
        if (is_reapt_event)
        {
            it->second = ActionState::HELD;
        }
        else
        {
            it->second = ActionState::PRESSED;
        }
    }
    else
    {
        it->second = ActionState::RELEASED;
    }
}

SDL_Scancode engine::input::InputManager::scancodeFromString(const std::string &key_name)
{
    return SDL_GetScancodeFromName(key_name.c_str());
}

Uint32 engine::input::InputManager::mouseButtonFromString(const std::string &button_name)
{
    if (button_name == "MouseLeft")
    {
        return SDL_BUTTON_LEFT;
    }
    else if (button_name == "MouseMiddle")
    {
        return SDL_BUTTON_MIDDLE;
    }
    else if (button_name == "MouseRight")
    {
        return SDL_BUTTON_RIGHT;
    }
    else if (button_name == "MouseX1")
    {
        return SDL_BUTTON_X1;
    }
    else if (button_name == "MouseX2")
    {
        return SDL_BUTTON_X2;
    }

    return 0;
}
