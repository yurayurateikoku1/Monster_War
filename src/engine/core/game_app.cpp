#include "game_app.h"
#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <spdlog/spdlog.h>
#include "time.h"
#include "context.h"
#include "game_state.h"
#include "../resource/resource_manager.h"
#include "../audio/audio_player.h"
#include "../render/camera.h"
#include "../render/text_renderer.h"
#include "../render/render.h"
#include "../input/input_manager.h"
#include "../component/transform_component.h"
#include "../component/sprite_component.h"
#include "../scene/scene_manager.h"
#include "config.h"
#include "../utils/events.h"
#include <entt/signal/dispatcher.hpp>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
namespace engine::core
{
    GameApp::GameApp()
    {
    }
    GameApp::~GameApp()
    {
        if (is_running_)
        {
            spdlog::warn("GameApp is still running,now is shutting down");
        }
        close();
    }

    void GameApp::run()
    {
        if (!init())
        {
            spdlog::error("GameApp init failed,now is shutting down");
            return;
        }
        while (is_running_)
        {
            time_->update();
            float dt = time_->getDeltaTime();
            handleEvents();
            update(dt);
            render();
            // 分发事件（让新创建的实体先更新再渲染）
            dispatcher_->update();
        }
        close();
    }

    void GameApp::registerSceneSutep(std::function<void(engine::core::Context &)> scene_setup_func)
    {
        scene_setup_func_ = std::move(scene_setup_func);
    }

    bool GameApp::iniDispatcher()
    {
        try
        {
            dispatcher_ = std::make_unique<entt::dispatcher>();
        }
        catch (const std::exception &e)
        {
            spdlog::error("Dispatcher init failed: {},{},{}", e.what(), __FILE__, __LINE__);
            return false;
        }

        return true;
    }

    bool GameApp::initConfig()
    {
        try
        {
            config_ = std::make_unique<engine::core::Config>("assets/config.json");
        }
        catch (const std::exception &e)
        {
            spdlog::error("Config init failed: {},{},{}", e.what(), __FILE__, __LINE__);
            return false;
        }
        spdlog::info("Config init success");
        return true;
    }

    bool GameApp::initSDL()
    {
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
        {
            spdlog::error("SDL_Init failed: {}", SDL_GetError());
            return false;
        }

        // 设置窗口大小 (窗口大小 * 窗口缩放比例)
        int window_width = static_cast<int>(static_cast<float>(config_->window_width_) * config_->window_scale_);
        int window_height = static_cast<int>(static_cast<float>(config_->window_height_) * config_->window_scale_);
        window_ = SDL_CreateWindow(config_->window_title_.c_str(), window_width, window_height, SDL_WINDOW_RESIZABLE);

        if (window_ == nullptr)
        {
            spdlog::error("SDL_CreateWindow failed: {}", SDL_GetError());
            return false;
        }
        sdl_renderer_ = SDL_CreateRenderer(window_, nullptr);
        if (sdl_renderer_ == nullptr)
        {
            spdlog::error("SDL_CreateRenderer failed: {}", SDL_GetError());
            return false;
        }

        SDL_SetRenderDrawBlendMode(sdl_renderer_, SDL_BLENDMODE_BLEND);

        int vsync_code = config_->vsync_enabled_ ? SDL_RENDERER_VSYNC_ADAPTIVE : SDL_RENDERER_VSYNC_DISABLED;
        SDL_SetRenderVSync(sdl_renderer_, vsync_code);

        // 设置逻辑分辨率 (窗口大小 * 逻辑缩放比例)
        int logical_width = static_cast<int>(static_cast<float>(config_->window_width_) * config_->window_logical_scale_);
        int logical_height = static_cast<int>(static_cast<float>(config_->window_height_) * config_->window_logical_scale_);
        SDL_SetRenderLogicalPresentation(sdl_renderer_, logical_width, logical_height, SDL_LOGICAL_PRESENTATION_LETTERBOX);
        return true;
    }

    bool GameApp::initTime()
    {
        try
        {
            time_ = std::make_unique<Time>();
        }
        catch (const std::exception &e)
        {
            spdlog::error("Time init failed: {},{},{}", e.what(), __FILE__, __LINE__);
            return false;
        }
        time_->setTargetFps(config_->target_fps_);
        return true;
    }

    bool GameApp::initResourceManager()
    {
        try
        {
            resource_manager_ = std::make_unique<engine::resource::ResourceManager>(sdl_renderer_);
        }
        catch (const std::exception &e)
        {
            spdlog::error("ResourceManager init failed: {},{},{}", e.what(), __FILE__, __LINE__);
        }
        resource_manager_->loadResource("assets/data/resource_mapping.json");
        return true;
    }

    bool GameApp::initAudioPlayer()
    {
        try
        {
            audio_player_ = std::make_unique<engine::audio::AudioPlayer>(resource_manager_.get());
        }
        catch (const std::exception &e)
        {
            spdlog::error("AudioPlayer init failed: {},{},{}", e.what(), __FILE__, __LINE__);
            return false;
        }

        return true;
    }

    bool GameApp::initRenderer()
    {
        try
        {
            renderer_ = std::make_unique<engine::render::Renderer>(sdl_renderer_, resource_manager_.get());
        }
        catch (const std::exception &e)
        {
            spdlog::error("Renderer init failed: {},{},{}", e.what(), __FILE__, __LINE__);
            return false;
        }

        return true;
    }

    bool GameApp::initTextRenderer()
    {
        try
        {
            text_renderer_ = std::make_unique<engine::render::TextRenderer>(sdl_renderer_, resource_manager_.get());
        }
        catch (const std::exception &e)
        {
            spdlog::error("TextRenderer init failed: {},{},{}", e.what(), __FILE__, __LINE__);
            return false;
        }
        return true;
    }

    bool GameApp::initCamera()
    {
        try
        {
            camera_ = std::make_unique<engine::render::Camera>(game_state_->getLogicalSize());
        }
        catch (const std::exception &e)
        {
            spdlog::error("Camera init failed: {},{},{}", e.what(), __FILE__, __LINE__);
            return false;
        }

        return true;
    }

    bool GameApp::initInputManager()
    {
        try
        {
            input_manager_ = std::make_unique<engine::input::InputManager>(sdl_renderer_, config_.get(), dispatcher_.get());
        }
        catch (const std::exception &e)
        {
            spdlog::error("InputManager init failed: {},{},{}", e.what(), __FILE__, __LINE__);
            return false;
        }
        return true;
    }

    bool GameApp::initGameState()
    {
        try
        {
            game_state_ = std::make_unique<engine::core::GameState>(window_, sdl_renderer_);
        }
        catch (const std::exception &e)
        {
            spdlog::error("GameState init failed: {},{},{}", e.what(), __FILE__, __LINE__);
            return false;
        }

        return true;
    }

    bool GameApp::initContext()
    {
        try
        {
            context_ = std::make_unique<engine::core::Context>(*dispatcher_, *input_manager_, *renderer_, *resource_manager_, *camera_,
                                                               *text_renderer_, *audio_player_, *game_state_);
        }
        catch (const std::exception &e)
        {
            spdlog::error("Context init failed: {},{},{}", e.what(), __FILE__, __LINE__);
            return false;
        }
        return true;
    }

    bool GameApp::initSceneManager()
    {
        try
        {
            scene_manager_ = std::make_unique<engine::scene::SceneManager>(*context_);
        }
        catch (const std::exception &e)
        {
            spdlog::error("SceneManager init failed: {},{},{}", e.what(), __FILE__, __LINE__);
            return false;
        }
        return true;
    }

    bool GameApp::initImGui()
    {
        // --- ImGui 步骤1 初始化 ---
        // ImGui必备初始化
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        /* 可选配置开始 */
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

        // 设置 ImGui 主题
        ImGui::StyleColorsDark();
        // ImGui::StyleColorsLight();

        // 设置缩放
        float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay()); // 与系统缩放一致
        // float main_scale = 1.0f;     // 或者直接设置更加稳定
        ImGuiStyle &style = ImGui::GetStyle();
        style.ScaleAllSizes(main_scale); // 固定样式缩放比例。
        style.FontScaleDpi = main_scale; // 设置初始字体缩放比例。

        // 设置透明度
        float window_alpha = 0.5f;

        // 修改各个UI元素的透明度
        style.Colors[ImGuiCol_WindowBg].w = window_alpha;
        style.Colors[ImGuiCol_PopupBg].w = window_alpha;

        // 为了正确显示中文，我们需要加载支持中文的字体。
        ImFont *font = io.Fonts->AddFontFromFileTTF(
            "assets/fonts/VonwaonBitmap-16px.ttf",            // 字体文件路径
            16.0f,                                            // 字体大小
            nullptr,                                          // 字体配置参数
            io.Fonts->GetGlyphRangesChineseSimplifiedCommon() // 字符范围
        );
        if (!font)
        {
            // 如果字体加载失败，回退到默认字体，但中文将无法显示。
            io.Fonts->AddFontDefault();
            spdlog::warn("Failed to load font, use default font instead");
        }
        /* 可选配置结束 */

        // 初始化 ImGui 的 SDL3 和 SDL_Renderer3 后端
        ImGui_ImplSDL3_InitForSDLRenderer(window_, sdl_renderer_);
        ImGui_ImplSDLRenderer3_Init(sdl_renderer_);

        spdlog::trace("ImGui init success");
        return true;
    }

    bool GameApp::init()
    {
        spdlog::info("GameApp init ...");
        if (!scene_setup_func_)
        {
            spdlog::error("Scene setup function is not set");
            return false;
        }
        if (!iniDispatcher())
        {
            return false;
        }

        if (!initConfig())
        {
            return false;
        }
        if (!initSDL())
        {
            return false;
        }
        if (!initGameState())
        {
            return false;
        }
        if (!initTime())
        {
            return false;
        }
        if (!initResourceManager())
        {
            return false;
        }
        if (!initAudioPlayer())
        {
            return false;
        }
        if (!initRenderer())
        {
            return false;
        }
        if (!initCamera())
        {
            return false;
        }
        if (!initInputManager())
        {
            return false;
        }
        if (!initTextRenderer())
        {
            return false;
        }

        if (!initContext())
        {
            return false;
        }
        if (!initSceneManager())
        {
            /* code */
            return false;
        }
        if (!initImGui())
            return false;
        // 调用场景设置函数
        scene_setup_func_(*context_);

        // 注册退出事件
        dispatcher_->sink<engine::utils::QuitEvent>().connect<&GameApp::onQuitEvent>(this);

        is_running_ = true;
        spdlog::info("GameApp init success");
        return true;
    }

    void GameApp::handleEvents()
    {
        input_manager_->update();
    }

    void GameApp::update([[maybe_unused]] float dt)
    {
        // 游戏逻辑更新
        scene_manager_->update(dt);
    }

    void GameApp::render()
    {
        renderer_->clearScreen();
        scene_manager_->render();
        renderer_->present();
    }

    void GameApp::close()
    {
        spdlog::info("GameApp close ...");

        ImGui_ImplSDLRenderer3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();

        // 断开事件处理函数
        dispatcher_->sink<engine::utils::QuitEvent>().disconnect<&GameApp::onQuitEvent>(this);

        // 先关闭场景管理器
        scene_manager_->close();

        // 确保正确的销毁顺序
        resource_manager_.reset();
        if (sdl_renderer_ != nullptr)
        {
            SDL_DestroyRenderer(sdl_renderer_);
            sdl_renderer_ = nullptr;
        }
        if (window_ != nullptr)
        {
            SDL_DestroyWindow(window_);
            window_ = nullptr;
        }

        SDL_Quit();
        is_running_ = false;
    }

    void GameApp::onQuitEvent()
    {
        spdlog::info("GameApp receive quit event");
        is_running_ = false;
    }
}