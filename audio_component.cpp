#include "audio_component.h"
#include "transform_component.h"
#include "../object/game_object.h"
#include "../audio/audio_player.h"
#include "../render/camera.h"
#include <spdlog/spdlog.h>
engine::component::AudioComponent::AudioComponent(engine::audio::AudioPlayer *audio_player, engine::render::Camera *camera)
    : audio_player_(audio_player), camera_(camera)
{
    if (!audio_player_ || !camera_)
    {
        spdlog::error("AudioComponent init failed.");
    }
}

void engine::component::AudioComponent::playerSound(const std::string &sound_id, int channel, bool use_spatial)
{
    auto sound_path = sound_id_to_path_.find(sound_id) != sound_id_to_path_.end() ? sound_id_to_path_[sound_id] : sound_id;
    if (use_spatial && transform_)
    {
        auto camera_center = camera_->getPosition() + camera_->getViewportSize() / 2.0f;
        auto object_position = transform_->getPosition();
        float distance = glm::length(camera_center - object_position);
        if (distance > 150.0f)
        {
            return;
        }
        audio_player_->playSound(sound_path, channel);
    }
    else
    {
        audio_player_->playSound(sound_path, channel);
    }
}

void engine::component::AudioComponent::addSound(const std::string &sound_id, const std::string &sound_path)
{
    if (sound_id_to_path_.find(sound_id) != sound_id_to_path_.end())
    {
    }
    sound_id_to_path_[sound_id] = sound_path;
}

void engine::component::AudioComponent::init()
{
    if (!owner_)
    {
        return;
    }
    transform_ = owner_->getComponent<engine::component::TransformComponent>();
    if (!transform_)
    {
        spdlog::warn("GameObject {} does not have a TransformComponent.", owner_->getName());
    }
}

void engine::component::AudioComponent::update(float dt, engine::core::Context &)
{
}
