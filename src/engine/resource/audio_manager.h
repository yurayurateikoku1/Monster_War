#pragma once
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>
#include <SDL3_mixer/SDL_mixer.h>
#include <entt/core/fwd.hpp>
namespace engine::resource
{

    /// @brief 音频管理器
    class AudioManager
    {
        friend class ResourceManager;

    public:
        AudioManager();
        AudioManager(const AudioManager &) = delete;
        AudioManager &operator=(const AudioManager &) = delete;
        AudioManager(AudioManager &&) = delete;
        AudioManager &operator=(AudioManager &&) = delete;
        ~AudioManager();

        MIX_Mixer *getMixer() const { return mixer_; }

    private:
        struct SDLAudioDeleter
        {
            void operator()(MIX_Audio *audio) const
            {
                if (audio)
                {
                    MIX_DestroyAudio(audio);
                }
            }
        };

        MIX_Mixer *mixer_ = nullptr;
        std::unordered_map<entt::id_type, std::unique_ptr<MIX_Audio, SDLAudioDeleter>> audios_;
        std::unordered_map<entt::id_type, std::unique_ptr<MIX_Audio, SDLAudioDeleter>> musics_;

    private:
        MIX_Audio *loadSound(entt::id_type id, const std::string &file_path);
        MIX_Audio *loadSound(entt::hashed_string str_hs);
        MIX_Audio *loadMusic(entt::id_type id, const std::string &file_path);
        MIX_Audio *loadMusic(entt::hashed_string str_hs);
        void unloadSound(entt::id_type id);
        void unloadMusic(entt::id_type id);
        MIX_Audio *getSound(entt::id_type id, const std::string &file_path);
        MIX_Audio *getSound(entt::hashed_string str_hs);
        MIX_Audio *getMusic(entt::id_type id, const std::string &file_path);
        MIX_Audio *getMusic(entt::hashed_string str_hs);
        void clearSounds();
        void clearMusics();
    };
}