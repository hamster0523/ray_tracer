#pragma once

#include "material.h"

class lambertian : public material
{
    private:
        color albedo;
        material_type type = material_type::LAMBERTIAN;

    public:
        lambertian(const color& albedo) : albedo(albedo) {}

        bool scatter(const ray& r_in, hit_record& record, 
        color& attenuation, ray& scattered) const override
        {
            // scatter_direction
            auto scatter_direction = normalize(record.normal) + random_unit_vector();
            
            // Check scatter_direction if not zero
            if(scatter_direction.near_zero())
                scatter_direction = normalize(record.normal);

            // scattered_ray
            scattered = ray(record.p, scatter_direction);
            // attenuation - decay
            attenuation = albedo;
            record.mat_type = type;
            return true;
        }
};