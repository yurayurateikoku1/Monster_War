#pragma once
#include <memory>
#include <vector>
#include "../utils/events.h"
namespace engine::core
{
    class Context;
}

namespace engine::scene
{
    class Scene;

    /// @brief 场景管理器,管理和切换场景
    class SceneManager final
    {
    private:
        engine::core::Context &context_;
        std::vector<std::unique_ptr<Scene>> scenes_stack_;

        /// @brief 待处理的动作
        enum class PendingAction
        {
            None,
            Push,
            Pop,
            Replace
        };
        PendingAction pending_action_{PendingAction::None};
        /// @brief 待处理的场景
        std::unique_ptr<Scene> pending_scene_;

    public:
        explicit SceneManager(engine::core::Context &context);
        ~SceneManager();

        SceneManager(const SceneManager &) = delete;
        SceneManager(SceneManager &&) = delete;
        SceneManager &operator=(const SceneManager &) = delete;
        SceneManager &operator=(SceneManager &&) = delete;

        Scene *getCurrentScene() const;
        engine::core::Context &getContext() const { return context_; }

        void update(float dt);
        void render();
        void handleInput();
        void close();

    private:
        void onPopScene();
        void onPushScene(engine::utils::PushSceneEvent &event);
        void onReplaceScene(engine::utils::ReplaceSceneEvent &event);

        void processPendingActions();
        void pushScene(std::unique_ptr<Scene> &&scene);
        void popScene();
        void replaceScene(std::unique_ptr<Scene> &&scene);
    };
}