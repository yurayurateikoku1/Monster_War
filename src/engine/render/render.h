#pragma once
#include "image.h"
#include <optional>
#include <glm/glm.hpp>
#include "../utils/math.h"
struct SDL_Renderer;
struct SDL_FRect;

namespace engine::resource
{
    class ResourceManager;
}

namespace engine::render
{
    class Camera;

    /// @brief 渲染器
    class Renderer final
    {
    private:
        /// @brief 指向主SDL渲染器的非拥有指针
        SDL_Renderer *renderer_{nullptr};
        /// @brief 指向资源管理器的非拥有指针
        engine::resource::ResourceManager *resource_manager_{nullptr};

    public:
        Renderer(SDL_Renderer *sdl_renderer, engine::resource::ResourceManager *resource_manager);
        Renderer(const Renderer &) = delete;
        Renderer &operator=(const Renderer &) = delete;
        Renderer(Renderer &&) = delete;
        Renderer &operator=(Renderer &&) = delete;

        /// @brief 绘制纹理
        /// @param camera
        /// @param sprite
        /// @param position
        /// @param scale
        /// @param angle
        void drawImage(const Camera &camera, const engine::render::Image &image, const glm::vec2 &position, const glm::vec2 &scale = {1.0f, 1.0f}, double angle = 0.0f);

        /// @brief 绘制UI
        /// @param sprite
        /// @param position
        /// @param size
        void drawUIImage(const engine::render::Image &image, const glm::vec2 &position, const std::optional<glm::vec2> &size = std::nullopt);

        void drawUIFillRect(const engine::utils::Rect &rect, const engine::utils::FColor &color);
        /// @brief 更新屏幕
        void present();
        /// @brief 清空屏幕
        void clearScreen();

        void setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255);
        void setDrawColorFloat(float r, float g, float b, float a = 1.0f);

        SDL_Renderer *getSDLRenderer() const { return renderer_; }

    private:
        /// @brief 获取精灵的源矩形
        /// @param sprite
        /// @return
        std::optional<SDL_FRect> getImageSrcRect(const Image &image);
        /// @brief 判断矩形是否在视口内
        /// @param camera
        /// @param rect
        /// @return
        bool isRectInViewport(const Camera &camera, const SDL_FRect &rect);
    };
}