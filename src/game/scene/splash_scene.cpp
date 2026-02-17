#include "splash_scene.h"
#include "title_scene.h"
#include "../../engine/core/context.h"
#include "../../engine/core/game_state.h"
#include "../../engine/render/render.h"
#include "../../engine/resource/resource_manager.h"
#include <spdlog/spdlog.h>

using namespace entt::literals;

namespace game::scene
{

    SplashScene::SplashScene(engine::core::Context &context)
        : engine::scene::Scene("SplashScene", context),
          logo_image_("assets/textures/pixel_empire_a.png")
    {
    }

    SplashScene::~SplashScene() = default;

    void SplashScene::init()
    {
        // 预加载logo纹理
        context_.getResourceManager().loadTexture("assets/textures/pixel_empire_a.png"_hs);
        // 设置黑色背景
        context_.getRender().setBgColorFloat(0.0f, 0.0f, 0.0f, 1.0f);

        engine::scene::Scene::init();
    }

    void SplashScene::update(float delta_time)
    {
        engine::scene::Scene::update(delta_time);

        elapsed_time_ += delta_time;
        if (elapsed_time_ >= display_duration_)
        {
            goToTitleScene();
        }
    }

    void SplashScene::render()
    {
        auto &renderer = context_.getRender();
        auto &resource_manager = context_.getResourceManager();
        auto logical_size = context_.getGameState().getLogicalSize();

        // 获取纹理尺寸
        auto texture_size = resource_manager.getTextureSize("assets/textures/pixel_empire_a.png"_hs);

        // 居中绘制logo
        glm::vec2 position = {
            (logical_size.x - texture_size.x) / 2.0f,
            (logical_size.y - texture_size.y) / 2.0f};

        renderer.drawUIImage(logo_image_, position);

        engine::scene::Scene::render();
    }

    void SplashScene::goToTitleScene()
    {
        requestReplaceScene(std::make_unique<game::scene::TitleScene>(context_));
    }

}
