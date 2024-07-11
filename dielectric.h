#pragma once

#include "material.h"

class dieletric : public material
{
    private:
        // shoot from pure air to some kind of material
        // Here is refraction_index is 1.0f / some_material_index
        double refraction_index;
        // Assume Here dielatric material only refract and not absord ray
        // No radiance decay
        albedo attenuation = color(1.0f);
        material_type type = material_type::DIELETRIC;

    public:
        // default dieletric param is 1
        dieletric() : refraction_index(1.0f) {}

        dieletric(double index) : refraction_index(index) {}

        bool scatter(const ray& r_in, hit_record& record,
            color& attenuation, ray& scattered) const override
            {
                attenuation = this->attenuation;
                record.mat_type = this->type;

                // Note : Here refract Assume that the light is hit from front face to back face
                // front_face = dot(normalize(r.direction()), outward_normal) < 0;
                double ri = record.front_face ? refraction_index : (1.0f / refraction_index);
                auto refract_direction = refract(r_in.direction(), record.normal, ri);
                // auto refract_direction_2 = refract2(normalize(r_in.direction()), normalize(record.normal), refraction_index);
                scattered = ray(record.p, refract_direction);
                return true;
            }

};