#pragma once

#include "rt.h"

class ImageTexture : public Texture
{
    private :
        TextureType type = TextureType::IMAGE_TEXTURE;
        rt_image image;
        bool if_flip_v = true;

    public :
        ImageTexture(const char* filename) : image(filename) {}

        color value(double u, double v, const point3& p) const override
        {
            if(image.height() == 0 || image.width() == 0) return color(0, 1, 1);

            u = clamp(u, 0.0, 1.0);
            v = if_flip_v ? 1.0 - clamp(v, 0.0, 1.0) : clamp(v, 0.0, 1.0);

            int i = static_cast<int>(u * image.width());
            int j = static_cast<int>(v * image.height());
            auto pixel = image.pixel_data(i, j);

            auto color_scale = 1.0 / 255.0;
            return color(
               pixel[0] * color_scale,
               pixel[1] * color_scale,
               pixel[2] * color_scale
            );
        }

        void set_flip_v(bool flip_v) { if_flip_v = flip_v; }
};