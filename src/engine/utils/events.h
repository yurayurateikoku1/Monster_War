#pragma once

namespace engine::scene
{
    class Scene;
}
namespace engine::utils
{
    /// @brief 退出事件
    struct QuitEvent
    {
        /* data */
    };

    /// @brief 弹出场景
    struct PopSceneEvent
    {
        /* data */
    };

    /// @brief 推入场景
    struct PushSceneEvent
    {
        /* data */
        std::unique_ptr<engine::scene::Scene> scene;
    };

    /// @brief 替换场景
    struct ReplaceSceneEvent
    {
        std::unique_ptr<engine::scene::Scene> scene;
        /* data */
    };

}