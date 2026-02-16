#pragma once
#include "../render/image.h"
#include <string>
#include <glm/vec2.hpp>
#include <utility>
namespace engine::component
{
    struct ParallaxComponent
    {
        glm::vec2 scroll_factor_{}; ///< @brief 滚动速度因子 (0=静止, 1=随相机移动, <1=比相机慢)
        glm::bvec2 repeat_{true};   ///< @brief 是否重复
        bool is_visible_{true};     ///< @brief 是否可见

        ParallaxComponent(glm::vec2 scroll_factor,
                          glm::bvec2 repeat = glm::bvec2(true, true),
                          bool is_visible = true) : scroll_factor_(std::move(scroll_factor)),
                                                    repeat_(std::move(repeat)),
                                                    is_visible_(is_visible) {}
    };

}