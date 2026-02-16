#pragma once
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <SDL3/SDL_render.h>
#include <glm/glm.hpp>
#include <entt/core/fwd.hpp>
namespace engine::resource
{
    /// @brief 贴图管理器
    class TextureManager
    {
        friend class ResourceManager;

    public:
        explicit TextureManager(SDL_Renderer *renderer);
        TextureManager(const TextureManager &) = delete;
        TextureManager &operator=(const TextureManager &) = delete;
        TextureManager(TextureManager &&) = delete;
        TextureManager &operator=(TextureManager &&) = delete;

    private:
        /// @brief 纹理管理器析构函数
        struct SDLTextureDeleter
        {
            void operator()(SDL_Texture *texture) const
            {
                SDL_DestroyTexture(texture);
            }
        };

        /// @brief 存储文件路径和指向管理纹理贴图的指针
        std::unordered_map<entt::id_type, std::unique_ptr<SDL_Texture, SDLTextureDeleter>> textures_;

        /// @brief 指向主SDL渲染器的非拥有指针
        SDL_Renderer *renderer_{nullptr};

        /// @brief 从文件加载纹理资源
        /// @param file_path
        /// @return
        SDL_Texture *loadTexture(entt::id_type id, const std::string &file_path);
        SDL_Texture *loadTexture(entt::hashed_string str_hs);

        /// @brief 卸载纹理资源
        /// @param file_path
        void unloadTexture(entt::id_type id);

        /// @brief 获取纹理资源
        /// @param file_path
        /// @return
        SDL_Texture *getTexture(entt::id_type id, const std::string &file_path);
        SDL_Texture *getTexture(entt::hashed_string str_hs);

        /// @brief 获取纹理尺寸
        /// @param file_path
        /// @return
        glm::vec2 getTextureSize(entt::id_type id, const std::string &file_path);
        glm::vec2 getTextureSize(entt::hashed_string str_hs);
        /// @brief 清除所有纹理资源
        void clearTextures();
    };
}