#include "health_component.h"
#include "../../engine/object/game_object.h"
#include <spdlog/spdlog.h>
#include <glm/common.hpp>
engine::component::HealthComponent::HealthComponent(int max_health, float invincible_duration)
    : max_health_(glm::max(max_health, 1)), invincible_duration_(invincible_duration), current_health_(max_health)
{
}

bool engine::component::HealthComponent::takeDamage(int damage_amount)
{
    if (damage_amount <= 0 || !isAlive())
    {
        return false;
    }
    if (is_invincible_)
    {
        return false;
    }
    current_health_ -= damage_amount;
    current_health_ = std::max(current_health_, 0);
    if (isAlive() && invincible_duration_ > 0.0f)
    {
        setInvincible(invincible_duration_);
    }
    return true;
}

void engine::component::HealthComponent::heal(int heal_amount)
{
    if (heal_amount <= 0 || !isAlive())
    {
        return;
    }
    current_health_ += heal_amount;
    current_health_ = std::min(current_health_, max_health_);
}

void engine::component::HealthComponent::setCurrentHealth(int new_health)
{
    current_health_ = glm::max(0, glm::min(current_health_, max_health_));
}

void engine::component::HealthComponent::setMaxHealth(int new_max_health)
{
    max_health_ = glm::max(new_max_health, 1);
    current_health_ = std::min(current_health_, max_health_);
}

void engine::component::HealthComponent::setInvincible(float duration)
{
    if (duration > 0)
    {
        is_invincible_ = true;
        invincible_timer_ = duration;
    }
    else
    {
        is_invincible_ = false;
        invincible_timer_ = 0.0f;
    }
}

void engine::component::HealthComponent::update(float delta_time, engine::core::Context &)
{
    if (is_invincible_)
    {
        invincible_timer_ -= delta_time;
        if (invincible_timer_ <= 0.0f)
        {
            is_invincible_ = false;
            invincible_timer_ = 0.0f;
        }
    }
}
