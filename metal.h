#pragma once

#include "rt.h"
#include "material.h"

class metal : public material
{
    public:
        metal() : albe(color(1.0f)), fuzzy_factor(1.0f) {}

        metal(const albedo& albe, double fuzzy) : albe(albe), fuzzy_factor(fuzzy) {}

        metal(const color& albedo) : albe(albedo) {}

        bool scatter(
            const ray& r_in, hit_record& record,
            color& attenuation, ray& scattered
        ) const override
        {
            vec3 reflected = reflect(r_in.direction(), record.normal);
            reflected = normalize(reflected) + fuzzy_factor * random_unit_vector();
            record.mat_type = mat_type;
            scattered = ray(record.p, reflected);
            attenuation = albe;
            // return true;
            // Check after fuzzy, the reflected ray is in the same side as the origin reflected ray
            return dot(normalize(record.normal), normalize(reflected)) > 0;
        }

    private:
        albedo albe;
        material_type mat_type = material_type::METAL;
        double fuzzy_factor;
};