#pragma once
#include "../../engine/scene/scene.h"
#include "../../engine/render/image.h"

namespace game::scene
{

    /// @brief 启动画面场景，显示工作室图标3秒后自动跳转到标题场景
    class SplashScene final : public engine::scene::Scene
    {
        engine::render::Image logo_image_;
        float elapsed_time_{0.0f};
        float display_duration_{3.0f}; ///< @brief 显示时长（秒）

    public:
        explicit SplashScene(engine::core::Context &context);
        ~SplashScene();

        void init() override;
        void update(float delta_time) override;
        void render() override;

    private:
        void goToTitleScene();
    };

}
