#pragma once
#include <memory>
#include <functional>
#include <entt/signal/fwd.hpp>
struct SDL_Window;
struct SDL_Renderer;
struct MIX_Mixer;
namespace engine::resource
{
    class ResourceManager;
}
namespace engine::render
{
    class Renderer;
    class Camera;
    class TextRenderer;
}
namespace engine::input
{
    class InputManager;
} // namespace engine::input

namespace engine::scene
{
    class SceneManager;
}
namespace engine::audio
{
    class AudioPlayer;
}
namespace engine::core
{
    class Time;
    class Config;
    class Context;
    class GameState;
    /// @brief 主应用程序,初始化SDL,运行主循环
    class GameApp final
    {
    private:
        SDL_Window *window_{nullptr};
        SDL_Renderer *sdl_renderer_{nullptr};
        MIX_Mixer *mixer_ = nullptr;
        bool is_running_{false};

        std::function<void(engine::core::Context &)> scene_setup_func_;

        std::unique_ptr<entt::dispatcher> dispatcher_; // 事件分发器
        std::unique_ptr<engine::core::Time> time_{nullptr};
        std::unique_ptr<engine::resource::ResourceManager> resource_manager_{nullptr};
        std::unique_ptr<engine::render::Renderer> renderer_{nullptr};
        std::unique_ptr<engine::render::Camera> camera_{nullptr};
        std::unique_ptr<engine::render::TextRenderer> text_renderer_{nullptr};
        std::unique_ptr<engine::core::Config> config_{nullptr};
        std::unique_ptr<engine::input::InputManager> input_manager_{nullptr};
        std::unique_ptr<engine::core::Context> context_{nullptr};
        std::unique_ptr<engine::scene::SceneManager> scene_manager_{nullptr};
        std::unique_ptr<engine::audio::AudioPlayer> audio_player_{nullptr};
        std::unique_ptr<engine::core::GameState> game_state_{nullptr};

    public:
        GameApp();
        ~GameApp();
        GameApp(const GameApp &) = delete;
        GameApp(GameApp &&) = delete;
        GameApp &operator=(const GameApp &) = delete;
        GameApp &operator=(GameApp &&) = delete;

        [[nodiscard]] bool init();
        void handleEvents();
        void update(float dt);
        void render();
        void close();

        void run();

        void registerSceneSutep(std::function<void(engine::core::Context &)> scene_setup_func);

        [[nodiscard]] bool iniDispatcher();
        [[nodiscard]] bool initConfig();
        [[nodiscard]] bool initSDL();
        [[nodiscard]] bool initTime();
        [[nodiscard]] bool initResourceManager();
        [[nodiscard]] bool initAudioPlayer();
        [[nodiscard]] bool initRenderer();
        [[nodiscard]] bool initTextRenderer();
        [[nodiscard]] bool initCamera();
        [[nodiscard]] bool initInputManager();
        [[nodiscard]] bool initGameState();
        [[nodiscard]] bool initContext();
        [[nodiscard]] bool initSceneManager();
        [[nodiscard]] bool initImGui();

        void onQuitEvent();
    };

}