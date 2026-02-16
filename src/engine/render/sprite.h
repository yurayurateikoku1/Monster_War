#pragma once
#include <SDL3/SDL_rect.h>
#include <optional>
#include <string>
#include <entt/core/hashed_string.hpp>
#include <entt/entity/entity.hpp>
#include "../utils/math.h"
namespace engine::render
{
    class Sprite
    {
    private:
        /// @brief 纹理文件路径
        std::string texture_path_;
        /// @brief 纹理ID
        entt::id_type texture_id_{entt::null};
        /// @brief 源矩形
        std::optional<engine::utils::Rect> source_rect_;
        /// @brief 是否翻转
        bool is_flipped_ = false;

    public:
        Sprite() = default;

        Sprite(const std::string &texture_path, std::optional<engine::utils::Rect> source_rect = std::nullopt, bool is_flipped = false);

        Sprite(entt::id_type texture_id, std::optional<engine::utils::Rect> source_rect = std::nullopt, bool is_flipped = false);

        const std::string &getTexturePath() const { return texture_path_; }
        entt::id_type getTextureId() const { return texture_id_; }
        const std::optional<engine::utils::Rect> &getSourceRect() const { return source_rect_; }
        bool isFlipped() const { return is_flipped_; }

        void setTexture(const std::string &texture_path)
        {
            texture_path_ = texture_path;
            texture_id_ = entt::hashed_string(texture_path.data());
        }
        void setTextureId(entt::id_type texture_id) { texture_id_ = texture_id; }
        void setSourceRect(const std::optional<engine::utils::Rect> &source_rect) { source_rect_ = source_rect; }
        void setFlipped(bool is_flipped) { is_flipped_ = is_flipped; }
    };

}