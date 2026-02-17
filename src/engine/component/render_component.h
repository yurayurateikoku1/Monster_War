#pragma once

namespace engine::component
{
    /// @brief 渲染组件,用于确定渲染顺序
    struct RenderComponent
    {
        int layer_{};
        float depth_{};
        RenderComponent(int layer = 0, float depth = 0)
            : layer_(layer), depth_(depth) {};
        bool operator<(const RenderComponent &other) const
        {
            if (layer_ == other.layer_)
            {
                return depth_ < other.depth_;
            }
            return layer_ < other.layer_;
        }
    };

}