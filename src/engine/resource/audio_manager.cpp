#include "audio_manager.h"
#include <algorithm>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <entt/core/hashed_string.hpp>
engine::resource::AudioManager::AudioManager()
{
    if (!MIX_Init())
    {
        throw std::runtime_error("SDL_mixer Init Failed" + std::string(SDL_GetError()));
        return;
    }
    SDL_AudioSpec spec{};
    spec.freq = 48000;
    spec.format = SDL_AUDIO_F32;
    spec.channels = 2;
    mixer_ = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec);
}

engine::resource::AudioManager::~AudioManager()
{
    clearSounds();
    clearMusics();

    if (mixer_)
    {
        MIX_DestroyMixer(mixer_);
        mixer_ = nullptr;
    }
    MIX_Quit();
}

MIX_Audio *engine::resource::AudioManager::loadSound(entt::id_type id, const std::string &file_path)
{
    // 检查是否已经加载
    auto it = audios_.find(id);
    if (it != audios_.end())
    {
        return it->second.get();
    }

    // 加载音频文件
    MIX_Audio *audio = MIX_LoadAudio(mixer_, file_path.data(), false);
    if (!audio)
    {
        spdlog::error("Failed to load sound: {} - {}", file_path, SDL_GetError());
        return nullptr;
    }

    // 存储到容器中
    audios_[id] = std::unique_ptr<MIX_Audio, SDLAudioDeleter>(audio);
    spdlog::info("Loaded sound: {}", file_path);
    return audio;
}

MIX_Audio *engine::resource::AudioManager::loadSound(entt::hashed_string str_hs)
{
    return loadSound(str_hs.value(), str_hs.data());
}

MIX_Audio *engine::resource::AudioManager::loadMusic(entt::id_type id, const std::string &file_path)
{
    // 检查是否已经加载
    auto it = musics_.find(id);
    if (it != musics_.end())
    {
        return it->second.get();
    }

    // 加载音乐文件 (预解码以减少播放时的 CPU 负载)
    MIX_Audio *music = MIX_LoadAudio(mixer_, file_path.data(), true);
    if (!music)
    {
        spdlog::error("Failed to load music: {} - {}", id, SDL_GetError());
        return nullptr;
    }

    // 存储到容器中
    musics_[id] = std::unique_ptr<MIX_Audio, SDLAudioDeleter>(music);
    spdlog::info("Loaded music: {}", id);
    return music;
}
MIX_Audio *engine::resource::AudioManager::loadMusic(entt::hashed_string str_hs)
{
    return loadMusic(str_hs.value(), str_hs.data());
}

void engine::resource::AudioManager::unloadSound(entt::id_type id)
{
    auto it = audios_.find(id);
    if (it != audios_.end())
    {
        audios_.erase(it);
        spdlog::info("Unloaded sound: {}", id);
    }
}

void engine::resource::AudioManager::unloadMusic(entt::id_type id)
{
    auto it = musics_.find(id);
    if (it != musics_.end())
    {
        musics_.erase(it);
        spdlog::info("Unloaded music: {}", id);
    }
}

MIX_Audio *engine::resource::AudioManager::getSound(entt::id_type id, const std::string &file_path)
{
    auto it = audios_.find(id);
    if (it != audios_.end())
    {
        return it->second.get();
    }
    if (!file_path.empty())
    {
        return loadSound(id, file_path);
    }
    return nullptr;
}

MIX_Audio *engine::resource::AudioManager::getSound(entt::hashed_string str_hs)
{
    return getSound(str_hs.value(), str_hs.data());
}

MIX_Audio *engine::resource::AudioManager::getMusic(entt::id_type id, const std::string &file_path)
{
    auto it = musics_.find(id);
    if (it != musics_.end())
    {
        return it->second.get();
    }
    if (!file_path.empty())
    {
        return loadMusic(id, file_path);
    }
    return nullptr;
}

MIX_Audio *engine::resource::AudioManager::getMusic(entt::hashed_string str_hs)
{
    return getMusic(str_hs.value(), str_hs.data());
}

void engine::resource::AudioManager::clearSounds()
{
    audios_.clear();
    spdlog::info("Cleared all sounds");
}

void engine::resource::AudioManager::clearMusics()
{
    musics_.clear();
    spdlog::info("Cleared all music");
}
