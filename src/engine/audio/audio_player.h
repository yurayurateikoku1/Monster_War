#pragma once
#include <string>
#include <entt/entity/fwd.hpp>
struct MIX_Track;
struct MIX_Audio;
struct MIX_Mixer;
namespace engine::resource
{
    class ResourceManager;
}
namespace engine::audio
{
    class AudioPlayer final
    {
    private:
        engine::resource::ResourceManager *resource_manager_;
        MIX_Mixer *mixer_ = nullptr;

        MIX_Track *musicTrack_ = nullptr;
        MIX_Track *soundTrack_ = nullptr;

        std::string current_music_;

    public:
        explicit AudioPlayer(engine::resource::ResourceManager *resource_manager);
        ~AudioPlayer();

        AudioPlayer(const AudioPlayer &) = delete;
        AudioPlayer(AudioPlayer &&) = delete;
        AudioPlayer &operator=(const AudioPlayer &) = delete;
        AudioPlayer &operator=(AudioPlayer &&) = delete;

        int playSound(entt::id_type sound_id, int channel = -1);

        int playSound(entt::hashed_string hashed_path, int channel = -1);

        int playMusic(entt::id_type music_id, int loops = -1, int fade_in_ms = 0);

        int playMusic(entt::hashed_string hashed_path, int loops = -1, int fade_in_ms = 0);

        void stopMusic(int fade_out_ms = 0);

        void pauseMusic();

        void resumeMusic();

        void setSoundVolume(float volume, int channel = -1);

        void setMusicVolume(float volume);

        float getMusicVolume();

        float getSoundVolume(int channel = -1);
    };
}
