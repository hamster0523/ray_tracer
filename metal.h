#pragma once

#include "rt.h"
#include "material.h"

class metal : public material
{
    public:
        metal(const color& albedo) : albedo(albedo) {}

        bool scatter(
            const ray& r_in, hit_record& record,
            color& attenuation, ray& scattered
        ) const override
        {
            vec3 reflected = reflect(r_in.direction(), record.normal);
            record.mat_type = mat_type;
            scattered = ray(record.p, reflected);
            attenuation = albedo;
            return true;
        }

    private:
        color albedo;
        material_type mat_type = material_type::METAL;
};