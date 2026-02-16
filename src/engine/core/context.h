#pragma once
#include <entt/signal/fwd.hpp>
namespace engine::input
{
    class InputManager;
}
namespace engine::render
{
    class Renderer;
    class Camera;
    class TextRenderer;
}

namespace engine::resource
{
    class ResourceManager;
}

namespace engine::audio
{
    class AudioPlayer;
}
namespace engine::core
{
    class GameState;
    /// @brief 持有对核心引擎模块引用的上下文对象
    /// 简化依赖注入，传递Context来获取引擎的各个模块
    class Context final
    {
    private:
        /// @brief 事件分发器
        entt::dispatcher &dispatcher_;
        /// @brief 输入管理器
        engine::input::InputManager &input_manager_;
        /// @brief 渲染器
        engine::render::Renderer &renderer_;
        /// @brief 资源管理器
        engine::resource::ResourceManager &resource_manager_;
        /// @brief 摄像机
        engine::render::Camera &camera_;

        engine::audio::AudioPlayer &audio_player_;
        engine::render::TextRenderer &text_renderer_;
        engine::core::GameState &game_state_;

    public:
        Context(entt::dispatcher &dispatcher,
                engine::input::InputManager &input_manager,
                engine::render::Renderer &render,
                engine::resource::ResourceManager &resource_manager,
                engine::render::Camera &camera,
                engine::render::TextRenderer &text_renderer,
                engine::audio::AudioPlayer &audio_player,
                engine::core::GameState &game_state);
        Context(const Context &) = delete;
        Context(Context &&) = delete;
        Context &operator=(const Context &) = delete;
        Context &operator=(Context &&) = delete;

        entt::dispatcher &getDispatcher() const { return dispatcher_; }
        engine::input::InputManager &getInputManager() const { return input_manager_; }
        engine::render::Renderer &getRender() const { return renderer_; }
        engine::resource::ResourceManager &getResourceManager() const { return resource_manager_; }
        engine::render::Camera &getCamera() const { return camera_; }
        engine::render::TextRenderer &getTextRenderer() const { return text_renderer_; }
        engine::audio::AudioPlayer &getAudioPlayer() const { return audio_player_; }
        engine::core::GameState &getGameState() const { return game_state_; }
    };
}