#include "tilelayer_component.h"
#include "../object/game_object.h"
#include "../core/context.h"
#include "../render/render.h"
#include "../render/camera.h"
#include <spdlog/spdlog.h>

engine::component::TileLayerComponent::TileLayerComponent(const glm::ivec2 &tile_size, const glm::ivec2 &map_size, std::vector<TileInfo> &&tiles)
    : tile_size_(tile_size), map_size_(map_size), tiles_(std::move(tiles))
{
    if (tiles_.size() != static_cast<size_t>(map_size_.x * map_size_.y))
    {
        /* code */
        spdlog::error("TileLayerComponent: tiles size is not equal to map size");
        tiles.clear();
        map_size_ = {0, 0};
    }
    spdlog::info("TileLayerComponent created");
}

const engine::component::TileInfo *engine::component::TileLayerComponent::getTileInfoAt(glm::ivec2 pos) const
{
    if (pos.x < 0 || pos.x >= map_size_.x || pos.y < 0 || pos.y >= map_size_.y)
    {
        // 越界是正常情况，返回 nullptr 表示空瓦片
        return nullptr;
    }
    size_t index = static_cast<size_t>(pos.y * map_size_.x + pos.x);

    if (index < tiles_.size())
    {
        return &tiles_[index];
    }

    return nullptr;
}

engine::component::TileType engine::component::TileLayerComponent::getTileTypeAt(glm::ivec2 pos) const
{
    const TileInfo *info = getTileInfoAt(pos);
    return info ? info->type : TileType::EMPTY;
}

engine::component::TileType engine::component::TileLayerComponent::getTileTypeAtWorldPos(const glm::vec2 &pos) const
{
    glm::vec2 relative_pos = pos - offset_;
    int tile_x = static_cast<int>(std::floor(relative_pos.x / tile_size_.x));
    int tile_y = static_cast<int>(std::floor(relative_pos.y / tile_size_.y));
    return getTileTypeAt({tile_x, tile_y});
}

void engine::component::TileLayerComponent::init()
{
    if (!owner_)
    {
        /* code */
        spdlog::warn("TileLayerComponent: Owner is not set.");
    }

    spdlog::info("TileLayerComponent initialized");
}

void engine::component::TileLayerComponent::render(engine::core::Context &context)
{
    if (tile_size_.x <= 0 || tile_size_.y <= 0)
    {
        /* code */
        return;
    }

    int rendered_count = 0;
    for (int y = 0; y < map_size_.y; ++y)
    {
        for (int x = 0; x < map_size_.x; ++x)
        {
            size_t index = static_cast<size_t>(y) * map_size_.x + x;
            // 检测索引有效性以瓦片是否需要渲染
            if (index < tiles_.size() && tiles_[index].type != TileType::EMPTY)
            {
                /* code */
                const auto &tile_info = tiles_[index];
                // 计算瓦片左上角的世界坐标
                glm::vec2 tile_left_top_pos = {
                    offset_.x + static_cast<float>(x) * tile_size_.x,
                    offset_.y + static_cast<float>(y) * tile_size_.y};
                // 如果瓦片高度不等于纹理高度 调整y坐标 瓦片层的对齐点是左下角
                const auto &source_rect = tile_info.sprite.getSourceRect();
                if (source_rect.has_value() && static_cast<int>(source_rect->h) != tile_size_.y)
                {
                    /* code */
                    tile_left_top_pos.y -= (tile_size_.y - source_rect->h) - static_cast<float>(tile_size_.y);
                }
                context.getRender().drawSprite(context.getCamera(), tile_info.sprite, tile_left_top_pos);
                rendered_count++;
            }
        }
    }
}
