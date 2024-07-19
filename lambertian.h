#pragma once

#include "material.h"
#include "rt.h"

class lambertian : public material
{
    private:
        color albedo;
        material_type type = material_type::LAMBERTIAN;
        // When happen lambertian scatter -> We use a texture
        std::shared_ptr<Texture> texture;

    public:
        lambertian(const color& albedo) : albedo(albedo)
        , texture(std::make_shared<SolidColor>(albedo)) {}

        lambertian(std::shared_ptr<Texture> texture) : texture(texture) {}

        bool scatter(const ray& r_in, hit_record& record, 
        color& attenuation, ray& scattered) const override
        {
            // scatter_direction
            auto scatter_direction = normalize(record.normal) + random_unit_vector();
            
            // Check scatter_direction if not zero
            if(scatter_direction.near_zero())
                scatter_direction = normalize(record.normal);

            // Update the ray time = r_in.time
            // scattered_ray
            // scattered = ray(record.p, scatter_direction);
            // -------------------------
            scattered = ray(record.p, scatter_direction, r_in.get_time());

            // using texture to get albedo
            // ------------------------
            // attenuation - decay
            // attenuation = albedo;
            //-------------------------
            attenuation = texture->value(record.u, record.v, record.p);
            record.mat_type = type;
            return true;
        }
};