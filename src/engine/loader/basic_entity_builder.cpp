#include "basic_entity_builder.h"
#include "level_loader.h"
#include "../core/context.h"
#include "../component/tilelayer_component.h"
#include "../component/name_component.h"
#include "../component/sprite_component.h"
#include "../component/transform_component.h"
#include "../resource/resource_manager.h"
#include <entt/entt.hpp>
#include <spdlog/spdlog.h>

engine::loader::BasicEntityBuilder::BasicEntityBuilder(engine::loader::LevelLoader &level_loader, engine::core::Context &context, entt::registry &registry)
    : level_loader_(level_loader), context_(context), registry_(registry)
{
}

engine::loader::BasicEntityBuilder::~BasicEntityBuilder()
{
}

engine::loader::BasicEntityBuilder *engine::loader::BasicEntityBuilder::configure(const nlohmann::json *object_json)
{
    reset();
    if (!object_json)
    {
        spdlog::error("failed to configure basic entity builder, object_json is nullptr");
        return nullptr;
    }
    object_json_ = object_json;
    spdlog::trace("configured basic entity builder");
    return this;
}

engine::loader::BasicEntityBuilder *engine::loader::BasicEntityBuilder::configure(const nlohmann::json *object_json, const engine::component::TileInfo *tile_info)
{
    reset();
    if (!object_json || !tile_info)
    {
        spdlog::error("failed to configure basic entity builder, object_json or tile_info is nullptr");
        return nullptr;
    }

    object_json_ = object_json;
    tile_info_ = tile_info;
    spdlog::trace("configured basic entity builder");
    return this;
}

engine::loader::BasicEntityBuilder *engine::loader::BasicEntityBuilder::configure(int index, const engine::component::TileInfo *tile_info)
{
    reset();
    if (!tile_info)
    {
        spdlog::error("failed to configure basic entity builder, tile_info is nullptr");
        return nullptr;
    }
    index_ = index;
    tile_info_ = tile_info;
    spdlog::trace("configured basic entity builder");
    return this;
}

engine::loader::BasicEntityBuilder *engine::loader::BasicEntityBuilder::build()
{
    if (!object_json_ && !tile_info_)
    {
        spdlog::error("object_json 和 tile_info 都为空，无法进行构建");
        return this;
    }

    // 按顺序构建各个组件
    buildBase();
    buildSprite();
    buildTransform();
    buildAnimation();
    buildAudio();
    return this;
}

entt::entity engine::loader::BasicEntityBuilder::getEntityID()
{
    return entity_id_;
}

void engine::loader::BasicEntityBuilder::reset()
{
    object_json_ = nullptr;
    tile_info_ = nullptr;
    index_ = -1;
    entity_id_ = entt::null;
    position_ = glm::vec2(0.0f);
    dst_size_ = glm::vec2(0.0f);
    src_size_ = glm::vec2(0.0f);
}

void engine::loader::BasicEntityBuilder::buildBase()
{
    spdlog::trace("create basic entity");
    // 创建一个实体并添加NameComponent组件
    entity_id_ = registry_.create();
    if (object_json_ && object_json_->contains("name"))
    {
        std::string name = object_json_->value("name", "");
        entt::id_type name_id = entt::hashed_string(name.c_str());
        registry_.emplace<engine::component::NameComponent>(entity_id_, name_id, name);
        spdlog::trace("create basic entity with name: {}", object_json_->value("name", ""));
    }
}

void engine::loader::BasicEntityBuilder::buildSprite()
{
    spdlog::trace("create sprite component");
    // 如果是自定义形状对象，则不需要SpriteComponent
    if (!tile_info_)
        return;
    // 创建Sprite时候确保纹理加载
    auto &resource_manager = context_.getResourceManager();
    resource_manager.loadTexture(tile_info_->sprite_.texture_id_, tile_info_->sprite_.texture_path_);
    registry_.emplace<engine::component::SpriteComponent>(entity_id_, tile_info_->sprite_);
}

void engine::loader::BasicEntityBuilder::buildTransform()
{
    spdlog::trace("create transform component");
    glm::vec2 scale = glm::vec2(1.0f);
    float rotation = 0.0f;

    // 对象层实体，位置、尺寸和旋转信息从 object_json_ 中获取
    if (object_json_)
    {
        position_ = glm::vec2(object_json_->value("x", 0.0f), object_json_->value("y", 0.0f));
        dst_size_ = glm::vec2(object_json_->value("width", 0.0f), object_json_->value("height", 0.0f));
        position_ = glm::vec2(position_.x, position_.y - dst_size_.y); // 图片对象的position需要进行调整(左下角到左上角)
        rotation = object_json_->value("rotation", 0.0f);
        // 如果是图片对象，需要调整缩放
        if (tile_info_)
        {
            src_size_ = glm::vec2(tile_info_->sprite_.src_rect_.size.x, tile_info_->sprite_.src_rect_.size.y);
            scale = dst_size_ / src_size_;
        }
    }

    // 瓦片层实体，通过index (Tiled瓦片层data数据的索引) 计算位置
    if (index_ >= 0)
    {
        auto map_size = level_loader_.getMapSize();
        auto tile_size = level_loader_.getTileSize();
        position_ = glm::vec2((index_ % map_size.x) * tile_size.x,
                              (index_ / map_size.x) * tile_size.y);
    }

    // 添加 TransformComponent
    registry_.emplace<engine::component::TransformComponent>(entity_id_, position_, scale, rotation);
}

void engine::loader::BasicEntityBuilder::buildAnimation()
{
    spdlog::trace("create animation component");
    // 如果存在动画，其信息已经解析并保存在tile_info_中
    if (tile_info_ && tile_info_->animation_)
    {
        // 创建动画map
        std::unordered_map<entt::id_type, engine::component::Animation> animations;
        auto animation_id = entt::hashed_string("tile"); // 图块动画名称默认为"tile"
        animations.emplace(animation_id, std::move(tile_info_->animation_.value()));
        // 通过动画map创建AnimationComponent，并添加
        registry_.emplace<engine::component::AnimationComponent>(entity_id_, std::move(animations), animation_id);
    }
}

void engine::loader::BasicEntityBuilder::buildAudio()
{
    spdlog::trace("create audio component");
}

template <typename T>
std::optional<T> engine::loader::BasicEntityBuilder::getTileProperty(const nlohmann::json &tile_json, const std::string &property_name)
{
    return level_loader_.getTileProperty<T>(tile_json, property_name);
}
