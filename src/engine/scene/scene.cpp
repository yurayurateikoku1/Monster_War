#include "scene.h"
#include <spdlog/spdlog.h>
#include "scene_manager.h"
#include "../object/game_object.h"
#include "../render/camera.h"
#include "../core/context.h"
#include "../core/game_state.h"
#include "../ui/ui_manager.h"
#include <algorithm>
#include "../utils/events.h"
#include <entt/signal/dispatcher.hpp>
engine::scene::Scene::Scene(const std::string &scene_name, engine::core::Context &context)
    : scene_name_(scene_name), context_(context), is_initialized_(false), ui_manager_(std::make_unique<engine::ui::UIManager>())
{
    spdlog::info("Scene {} created", scene_name_);
}

engine::scene::Scene::~Scene() = default;

void engine::scene::Scene::init()
{
    is_initialized_ = true;
    spdlog::info("Scene {} initialized", scene_name_);
}

void engine::scene::Scene::update(float dt)
{
    if (!is_initialized_)
    {
        return;
    }
    if (context_.getGameState().isPlaying())
    {
        context_.getCamera().update(dt);
    }

    // 更新所有游戏对象
    for (auto it = game_objects_.begin(); it != game_objects_.end();)
    {
        if (*it && !(*it)->getNeedRemove())
        {
            (*it)->update(dt, context_);
            ++it;
        }
        else
        {
            if (*it)
            {
                (*it)->clean();
            }
            it = game_objects_.erase(it);
        }
    }
    ui_manager_->update(dt, context_);
    processPendingAdditions();
}

void engine::scene::Scene::render()
{
    if (!is_initialized_)
    {
        /* code */
        return;
    }
    for (const auto &game_object : game_objects_)
    {
        if (game_object)
        {
            game_object->render(context_);
        }
    }
    ui_manager_->render(context_);
}

void engine::scene::Scene::handleInput()
{
    if (!is_initialized_)
    {
        return;
    }
    if (ui_manager_->handleInput(context_))
    {
        return;
    }

    for (auto it = game_objects_.begin(); it != game_objects_.end();)
    {
        if (*it && !(*it)->getNeedRemove())
        {
            (*it)->handleInput(context_);
            ++it;
        }
        else
        {
            if (*it)
            {
                (*it)->clean();
            }
            it = game_objects_.erase(it);
        }
    }
}

void engine::scene::Scene::clean()
{
    if (!is_initialized_)
    {
        /* code */
        return;
    }
    for (const auto &game_object : game_objects_)
    {
        if (game_object)
        {
            game_object->clean();
        }
    }
    game_objects_.clear();
    is_initialized_ = false;
    spdlog::info("Scene {} cleaned", scene_name_);
}

void engine::scene::Scene::addGameObject(std::unique_ptr<engine::object::GameObject> &&game_object)
{
    if (game_object)
    {
        /* code */
        game_objects_.push_back(std::move(game_object));
    }
    else
        spdlog::warn("{} scene add game object is nullptr", scene_name_);
}

void engine::scene::Scene::safeAddGameObject(std::unique_ptr<engine::object::GameObject> &&game_object)
{
    if (game_object)
    {
        pending_additions_.push_back(std::move(game_object));
    }
    else
        spdlog::warn("{} scene add game object is nullptr", scene_name_);
}

void engine::scene::Scene::removeGameObject(engine::object::GameObject *game_object_ptr)
{
    if (!game_object_ptr)
    {
        /* code */
        spdlog::warn("{} scene remove game object is nullptr", scene_name_);
        return;
    }
    auto it = std::remove_if(game_objects_.begin(), game_objects_.end(),
                             [game_object_ptr](std::unique_ptr<engine::object::GameObject> &obj)
                             { return obj.get() == game_object_ptr; });
    if (it != game_objects_.end())
    {
        /* code */
        (*it)->clean();
        game_objects_.erase(it, game_objects_.end());
        spdlog::info("{} scene remove game object", scene_name_);
    }
    else
    {
        spdlog::warn("{} scene remove game object not found", scene_name_);
    }
}

void engine::scene::Scene::safeRemoveGameObject(engine::object::GameObject *game_object_ptr)
{
    game_object_ptr->setNeedRemove(true);
}

engine::object::GameObject *engine::scene::Scene::findGameObjectByName(const std::string &name) const
{
    for (const auto &game_object : game_objects_)
    {
        /* code */
        if (game_object && game_object->getName() == name)
        {
            /* code */
            return game_object.get();
        }
    }
    return nullptr;
}

void engine::scene::Scene::requestPopScene()
{
    context_.getDispatcher().trigger<engine::utils::PopSceneEvent>();
}

void engine::scene::Scene::requestPushScene(std::unique_ptr<engine::scene::Scene> &&scene)
{
    context_.getDispatcher().trigger<engine::utils::PushSceneEvent>(engine::utils::PushSceneEvent{std::move(scene)});
}

void engine::scene::Scene::requestReplaceScene(std::unique_ptr<engine::scene::Scene> &&scene)
{
    context_.getDispatcher().trigger<engine::utils::ReplaceSceneEvent>(engine::utils::ReplaceSceneEvent{std::move(scene)});
}

void engine::scene::Scene::requestQuit()
{
    context_.getDispatcher().trigger<engine::utils::QuitEvent>();
}

void engine::scene::Scene::processPendingAdditions()
{
    for (auto &game_object : pending_additions_)
    {
        /* code */
        game_objects_.push_back(std::move(game_object));
    }
    pending_additions_.clear();
}
