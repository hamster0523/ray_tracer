#pragma once

#include "rt.h"

class CheckerTexture : public Texture
{
    private:
        TextureType texture_type = TextureType::CHECKER_TEXTURE;
        double inv_scale;
        // Single Color Texture -> even
        std::shared_ptr<Texture> even;
        // Single Color Texture -> odd
        std::shared_ptr<Texture> odd;

    public:
        // Shared_Ptr Use Copy to make sure the count is correct when use texture
        CheckerTexture(double scale, std::shared_ptr<Texture> even, std::shared_ptr<Texture> odd)
        : inv_scale(1.0 / scale), even(even), odd(odd) {}

        CheckerTexture(double scale, 
        const color& c1, const color& c2) :
        inv_scale(1.0 / scale),
        even(std::make_shared<SolidColor>(c1)),
        odd(std::make_shared<SolidColor>(c2)) {}


        color value(double u, double v, const vec3& p) const override
        {
            auto xInteger = static_cast<int>(std::floor(inv_scale * p.x()));
            auto yInteger = static_cast<int>(std::floor(inv_scale * p.y()));
            auto zInteger = static_cast<int>(std::floor(inv_scale * p.z()));

            bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;

            return isEven ? even->value(u, v, p) : odd->value(u, v, p);
        }
};