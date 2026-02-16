#pragma once
#include "../render/image.h"
#include "component.h"
#include "../utils/alignment.h"
#include <optional>
#include <SDL3/SDL_rect.h>
#include <glm/vec2.hpp>
namespace engine::core
{
    class Context;
}

namespace engine::resource
{
    class ResourceManager;
}

namespace engine::component
{
    class TransformComponent;

    class SpriteComponent final : public engine::component::Component
    {
        friend class engine::object::GameObject;

    private:
        engine::resource::ResourceManager *resourceManager_{nullptr};
        TransformComponent *transform_{nullptr};

        /// @brief 纹理对象
        engine::render::Image sprite_;
        /// @brief 对齐方式
        engine::utils::Alignment alignment_ = engine::utils::Alignment::NONE;
        glm::vec2 sprite_size_{0.0f, 0.0f};
        glm::vec2 offset_{0.0f, 0.0f};
        /// @brief 是否隐藏
        bool is_hidden_ = false;

    public:
        SpriteComponent(const std::string &texture_id,
                        engine::resource::ResourceManager &resource_manager,
                        engine::utils::Alignment alignment = engine::utils::Alignment::NONE,
                        std::optional<SDL_Rect> source_rect = std::nullopt,
                        bool is_flipped = false);

        SpriteComponent(engine::render::Image &&sprite,
                        engine::resource::ResourceManager &resource_manager,
                        engine::utils::Alignment alignment = engine::utils::Alignment::NONE);
        ~SpriteComponent() override = default;
        SpriteComponent(const SpriteComponent &) = delete;
        SpriteComponent &operator=(const SpriteComponent &) = delete;
        SpriteComponent(SpriteComponent &&) = delete;
        SpriteComponent &operator=(SpriteComponent &&) = delete;

        /// @brief 更新偏移量(根据alignment和sprite_size计算offset)
        void updateOffset();

        const engine::render::Image &getSprite() const { return sprite_; }
        entt::id_type getTextureId() const { return sprite_.getTextureId(); }
        bool isFlipped() const { return sprite_.isFlipped(); }
        bool isHidden() const { return is_hidden_; }
        const glm::vec2 &getOffset() const { return offset_; }
        const glm::vec2 &getSpriteSize() const { return sprite_size_; }
        engine::utils::Alignment getAlignment() const { return alignment_; }

        void setSpriteById(const std::string &texture_id, const std::optional<SDL_Rect> &source_rect = std::nullopt);
        void setSourceRect(const std::optional<SDL_Rect> &source_rect_opt);
        void setFlipped(bool is_flipped) { sprite_.setFlipped(is_flipped); };
        void setAlignment(engine::utils::Alignment anchor);
        void setHidden(bool is_hidden) { is_hidden_ = is_hidden; };

    private:
        void updateSpriteSize();

        void init() override;
        void update(float dt, engine::core::Context &context) override {};
        void render(engine::core::Context &context) override;
    };

}
