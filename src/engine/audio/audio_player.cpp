#include "audio_player.h"
#include "../resource/resource_manager.h"
#include <SDL3_mixer/SDL_mixer.h>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <entt/core/hashed_string.hpp>

engine::audio::AudioPlayer::AudioPlayer(engine::resource::ResourceManager *resource_manager)
    : resource_manager_(resource_manager)
{
    if (!resource_manager_)
    {
        throw std::runtime_error("ResourceManager is nullptr");
    }
    mixer_ = resource_manager_->getMixer();
    if (!mixer_)
    {
        throw std::runtime_error("MIX_Mixer is nullptr");
    }
}

engine::audio::AudioPlayer::~AudioPlayer()
{
    if (musicTrack_)
    {
        MIX_StopTrack(musicTrack_, 0);
        MIX_DestroyTrack(musicTrack_);
        musicTrack_ = nullptr;
    }
    if (soundTrack_)
    {
        MIX_StopTrack(soundTrack_, 0);
        MIX_DestroyTrack(soundTrack_);
        soundTrack_ = nullptr;
    }
}

int engine::audio::AudioPlayer::playSound(entt::id_type sound_id, int channel)
{
    MIX_Audio *audio = resource_manager_->getSound(sound_id);
    if (!audio)
    {
        spdlog::error("Sound not found for id: {}", sound_id);
        return -1;
    }

    if (!soundTrack_)
    {
        soundTrack_ = MIX_CreateTrack(mixer_);
        if (!soundTrack_)
        {
            spdlog::error("Failed to create sound track: {}", SDL_GetError());
            return -1;
        }
    }

    if (!MIX_SetTrackAudio(soundTrack_, audio))
    {
        spdlog::error("Failed to set track audio: {}", SDL_GetError());
        return -1;
    }

    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, 0);

    if (!MIX_PlayTrack(soundTrack_, props))
    {
        spdlog::error("Failed to play sound track: {}", SDL_GetError());
        SDL_DestroyProperties(props);
        return -1;
    }

    SDL_DestroyProperties(props);
    return 0;
}

int engine::audio::AudioPlayer::playSound(entt::hashed_string hashed_path, int channel)
{
    MIX_Audio *audio = resource_manager_->getSound(hashed_path);
    if (!audio)
    {
        spdlog::error("Sound not found: {}", hashed_path.data());
        return -1;
    }

    if (!soundTrack_)
    {
        soundTrack_ = MIX_CreateTrack(mixer_);
        if (!soundTrack_)
        {
            spdlog::error("Failed to create sound track: {}", SDL_GetError());
            return -1;
        }
    }

    if (!MIX_SetTrackAudio(soundTrack_, audio))
    {
        spdlog::error("Failed to set track audio: {}", SDL_GetError());
        return -1;
    }

    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, 0);

    if (!MIX_PlayTrack(soundTrack_, props))
    {
        spdlog::error("Failed to play sound track: {}", SDL_GetError());
        SDL_DestroyProperties(props);
        return -1;
    }

    SDL_DestroyProperties(props);
    return 0;
}

int engine::audio::AudioPlayer::playMusic(entt::id_type music_id, int loops, int fade_in_ms)
{
    MIX_Audio *audio = resource_manager_->getMusic(music_id);
    if (!audio)
    {
        spdlog::error("Music not found for id: {}", music_id);
        return -1;
    }

    if (!musicTrack_)
    {
        musicTrack_ = MIX_CreateTrack(mixer_);
        if (!musicTrack_)
        {
            spdlog::error("Failed to create music track: {}", SDL_GetError());
            return -1;
        }
    }

    if (!MIX_SetTrackAudio(musicTrack_, audio))
    {
        spdlog::error("Failed to set track audio: {}", SDL_GetError());
        return -1;
    }

    MIX_SetTrackGain(musicTrack_, 0.05f);

    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, loops);
    if (fade_in_ms > 0)
    {
        SDL_SetNumberProperty(props, MIX_PROP_PLAY_FADE_IN_MILLISECONDS_NUMBER, fade_in_ms);
    }

    if (!MIX_PlayTrack(musicTrack_, props))
    {
        spdlog::error("Failed to play music track: {}", SDL_GetError());
        SDL_DestroyProperties(props);
        return -1;
    }

    SDL_DestroyProperties(props);
    return 0;
}

int engine::audio::AudioPlayer::playMusic(entt::hashed_string hashed_path, int loops, int fade_in_ms)
{
    MIX_Audio *audio = resource_manager_->getMusic(hashed_path);
    if (!audio)
    {
        spdlog::error("Music not found: {}", hashed_path.data());
        return -1;
    }

    if (!musicTrack_)
    {
        musicTrack_ = MIX_CreateTrack(mixer_);
        if (!musicTrack_)
        {
            spdlog::error("Failed to create music track: {}", SDL_GetError());
            return -1;
        }
    }

    if (!MIX_SetTrackAudio(musicTrack_, audio))
    {
        spdlog::error("Failed to set track audio: {}", SDL_GetError());
        return -1;
    }

    MIX_SetTrackGain(musicTrack_, 0.05f);

    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, loops);
    if (fade_in_ms > 0)
    {
        SDL_SetNumberProperty(props, MIX_PROP_PLAY_FADE_IN_MILLISECONDS_NUMBER, fade_in_ms);
    }

    if (!MIX_PlayTrack(musicTrack_, props))
    {
        spdlog::error("Failed to play music track: {}", SDL_GetError());
        SDL_DestroyProperties(props);
        return -1;
    }

    SDL_DestroyProperties(props);
    return 0;
}

void engine::audio::AudioPlayer::stopMusic(int fade_out_ms)
{
    if (musicTrack_)
    {
        MIX_StopTrack(musicTrack_, fade_out_ms);
    }
}

void engine::audio::AudioPlayer::pauseMusic()
{
    if (musicTrack_)
    {
        MIX_PauseTrack(musicTrack_);
    }
}

void engine::audio::AudioPlayer::resumeMusic()
{
    if (musicTrack_)
    {
        MIX_ResumeTrack(musicTrack_);
    }
}

void engine::audio::AudioPlayer::setSoundVolume(float volume, int channel)
{
    if (soundTrack_)
    {
        MIX_SetTrackGain(soundTrack_, volume);
    }
}

void engine::audio::AudioPlayer::setMusicVolume(float volume)
{
    if (musicTrack_)
    {
        MIX_SetTrackGain(musicTrack_, volume);
    }
}

float engine::audio::AudioPlayer::getMusicVolume()
{
    if (musicTrack_)
    {
        return MIX_GetTrackGain(musicTrack_);
    }
    return 0.0f;
}

float engine::audio::AudioPlayer::getSoundVolume(int channel)
{
    if (soundTrack_)
    {
        return MIX_GetTrackGain(soundTrack_);
    }
    return 0.0f;
}
