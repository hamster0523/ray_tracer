#pragma once

#include "rt.h"

enum TextureType
{
    SOLID_TEXTURE,
    CHECKER_TEXTURE,
    IMAGE_TEXTURE,
};

class Texture
{
    public:
        virtual ~Texture() = default;
        virtual color value(double u, double v, const vec3& p) const = 0;
};
