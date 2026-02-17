#pragma once
#include <entt/entity/entity.hpp>
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

    /// @brief 播放动画事件
    struct PlayAnimationEvent
    {
        entt::entity entity_{entt::null};        ///< @brief 目标实体
        entt::id_type animation_id_{entt::null}; ///< @brief 动画ID
        bool loop_{true};                        ///< @brief 是否循环
    };

}