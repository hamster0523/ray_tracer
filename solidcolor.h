#pragma once

#include "rt.h"

// Pure Color Texture
class SolidColor : public Texture
{
    private:
        TextureType texture_type = TextureType::SOLID_TEXTURE;
        color albedo;
    
    public:
        SolidColor(const color& albedo) : albedo(albedo) {}

        SolidColor(double red, double green, double blue) : albedo(color(red, green, blue)) {}

        // Single color texture -> No use u, v
        color value(double u, double v, const vec3& p) const override
        {
            return albedo;
        }
};