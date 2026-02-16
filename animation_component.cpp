#include "animation_component.h"
#include "sprite_component.h"
#include "../object/game_object.h"
#include "../render/animation.h"

engine::component::AnimationComponent::~AnimationComponent() = default;

void engine::component::AnimationComponent::init()
{
    if (!owner_)
    {
        spdlog::error("AnimationComponent not has owner");
        return;
    }
    sprite_component_ = owner_->getComponent<engine::component::SpriteComponent>();
    if (!sprite_component_)
    {
        spdlog::error("AnimationComponent not has sprite component");
        return;
    }
}

void engine::component::AnimationComponent::addAnimation(std::unique_ptr<engine::render::Animation> &&animation)
{
    if (!animation)
        return;
    std::string name = animation->getName();
    animations_[name] = std::move(animation);
}

void engine::component::AnimationComponent::playAnimation(const std::string &name)
{
    auto it = animations_.find(name);
    if (it == animations_.end())
    { /* ... 错误处理 ... */
        return;
    }

    // 如果已经在播放相同的动画，则不重新开始
    if (current_animation_ == it->second.get() && is_playing_)
    {
        return;
    }

    current_animation_ = it->second.get();
    animation_timer_ = 0.0f; // 重置计时器
    is_playing_ = true;

    // 立即将精灵更新到第一帧
    if (sprite_component_ && !current_animation_->isEmpty())
    {
        const auto &first_frame = current_animation_->getFrame(0.0f);
        sprite_component_->setSourceRect(first_frame.source_rect);
    }
}

std::string engine::component::AnimationComponent::getCurrentAnimationName() const
{
    if (current_animation_)
    {
        return current_animation_->getName();
    }

    return std::string();
}

bool engine::component::AnimationComponent::isAnimationFinished() const
{
    if (!current_animation_ || current_animation_->isLooping())
    {
        return false;
    }
    return animation_timer_ >= current_animation_->getTotalDuration();
}

void engine::component::AnimationComponent::update(float delta_time, engine::core::Context &)
{
    if (!is_playing_ || !current_animation_ || !sprite_component_ || current_animation_->isEmpty())
    {
        return;
    }

    // 推进计时器
    animation_timer_ += delta_time;

    // 根据时间获取当前帧
    const auto &current_frame = current_animation_->getFrame(animation_timer_);

    // 更新精灵组件的源矩形
    sprite_component_->setSourceRect(current_frame.source_rect);
    if (!current_animation_->isLooping() && animation_timer_ >= current_animation_->getTotalDuration())
    {
        is_playing_ = false;
        animation_timer_ = current_animation_->getTotalDuration();
        if (is_one_shot_removal_)
        {
            owner_->setNeedRemove(true);
        }
    }
}
