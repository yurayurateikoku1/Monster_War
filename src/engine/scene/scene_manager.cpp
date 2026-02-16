#include "scene_manager.h"
#include <spdlog/spdlog.h>
#include "scene.h"
#include "../core/context.h"
#include "../utils/events.h"
#include <entt/signal/dispatcher.hpp>
engine::scene::SceneManager::SceneManager(engine::core::Context &context)
    : context_(context)
{
    context_.getDispatcher().sink<engine::utils::PopSceneEvent>().connect<&SceneManager::onPopScene>(this);
    context_.getDispatcher().sink<engine::utils::PushSceneEvent>().connect<&SceneManager::onPushScene>(this);
    context_.getDispatcher().sink<engine::utils::ReplaceSceneEvent>().connect<&SceneManager::onReplaceScene>(this);
    spdlog::info("SceneManager created");
}

engine::scene::SceneManager::~SceneManager()
{
    spdlog::info("SceneManager destroyed");
    close();
}

engine::scene::Scene *engine::scene::SceneManager::getCurrentScene() const
{
    if (scenes_stack_.empty())
        return nullptr;

    return scenes_stack_.back().get();
}

void engine::scene::SceneManager::update(float dt)
{
    Scene *current_scene = getCurrentScene();
    if (current_scene)
    {
        current_scene->update(dt);
    }
    processPendingActions();
}

void engine::scene::SceneManager::render()
{
    for (const auto &scene : scenes_stack_)
    {
        /* code */
        if (scene)
            scene->render();
    }
}

void engine::scene::SceneManager::handleInput()
{
    Scene *current_scene = getCurrentScene();
    if (current_scene)
    {
        current_scene->handleInput();
    }
}

void engine::scene::SceneManager::close()
{
    spdlog::info("SceneManager closed");
    while (!scenes_stack_.empty())
    {
        /* code */
        if (scenes_stack_.back())
        {
            /* code */
            spdlog::info("Scene {} closed", scenes_stack_.back()->getName());
            scenes_stack_.back()->clean();
        }
        scenes_stack_.pop_back();
    }

    context_.getDispatcher().disconnect(this);
}

void engine::scene::SceneManager::onPopScene()
{
    pending_action_ = PendingAction::Pop;
}

void engine::scene::SceneManager::onPushScene(engine::utils::PushSceneEvent &event)
{
    pending_action_ = PendingAction::Push;
    pending_scene_ = std::move(event.scene);
}

void engine::scene::SceneManager::onReplaceScene(engine::utils::ReplaceSceneEvent &event)
{
    pending_action_ = PendingAction::Replace;
    pending_scene_ = std::move(event.scene);
}

void engine::scene::SceneManager::processPendingActions()
{
    if (pending_action_ == PendingAction::None)
    {
        /* code */
        return;
    }
    switch (pending_action_)
    {
    case PendingAction::Pop:
        /* code */
        popScene();
        break;
    case PendingAction::Push:
        /* code */
        pushScene(std::move(pending_scene_));
        break;
    case PendingAction::Replace:
        /* code */
        replaceScene(std::move(pending_scene_));
        break;
    default:
        break;
    }
    pending_action_ = PendingAction::None;
}

void engine::scene::SceneManager::pushScene(std::unique_ptr<Scene> &&scene)
{
    if (!scene)
    {
        /* code */
        spdlog::warn("Scene is nullptr");
        return;
    }
    spdlog::info("Scene {} pushed", scene->getName());
    if (!scene->getInitialized())
    {
        /* code */
        scene->init();
    }
    scenes_stack_.push_back(std::move(scene));
}

void engine::scene::SceneManager::popScene()
{
    if (scenes_stack_.empty())
    {
        spdlog::warn("Scene stack is empty");
        return;
    }
    spdlog::info("Scene {} popped", scenes_stack_.back()->getName());
    if (scenes_stack_.back())
    {
        /* code */
        scenes_stack_.back()->clean();
    }
    scenes_stack_.pop_back();

    if (scenes_stack_.empty())
    {
        context_.getDispatcher().enqueue<engine::utils::QuitEvent>();
    }
}

void engine::scene::SceneManager::replaceScene(std::unique_ptr<Scene> &&scene)
{
    if (!scene)
    {
        /* code */
        spdlog::warn("Scene is nullptr");
        return;
    }
    spdlog::info("Scene {} replaced", scene->getName());
    while (!scenes_stack_.empty())
    {
        /* code */
        if (scenes_stack_.back())
        {
            /* code */
            scenes_stack_.back()->clean();
        }
        scenes_stack_.pop_back();
    }
    if (!scene->getInitialized())
    {
        /* code */
        scene->init();
    }
    scenes_stack_.push_back(std::move(scene));
}
