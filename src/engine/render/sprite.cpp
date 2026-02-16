#include "sprite.h"

engine::render::Sprite::Sprite(const std::string &texture_path, std::optional<engine::utils::Rect> source_rect, bool is_flipped)
    : texture_path_(texture_path), texture_id_(entt::hashed_string(texture_path.data())), source_rect_(source_rect), is_flipped_(is_flipped)
{
}

engine::render::Sprite::Sprite(entt::id_type texture_id, std::optional<engine::utils::Rect> source_rect, bool is_flipped)
    : texture_id_(texture_id), source_rect_(source_rect), is_flipped_(is_flipped)
{
}