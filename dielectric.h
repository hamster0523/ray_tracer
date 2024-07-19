#pragma once

#include "material.h"

class dieletric : public material
{
    private:
        // shoot from pure air to some kind of material
        // Here is refraction_index is 1.0f / some_material_index
        double refraction_index;
        // Assume Here dielatric material only refract and not absord ray
        // No radiance decay -> absord nothing
        albedo attenuation = color(1.0f);
        material_type type = material_type::DIELETRIC;

    public:
        // default dieletric param is 1
        dieletric() : refraction_index(1.0f) {}

        dieletric(double index) : refraction_index(index) {}

        bool scatter_2(const ray& r_in, hit_record& record,
            color& attenuation, ray& scattered) const 
            {
                attenuation = this->attenuation;
                record.mat_type = this->type;

                // Note : Here refract Assume that the light is hit from front face to back face
                // front_face = dot(normalize(r.direction()), outward_normal) < 0;
                double ri = record.front_face ? refraction_index : (1.0f / refraction_index);
                auto refract_direction = refract(r_in.direction(), record.normal, ri);
                // auto refract_direction = refract2(normalize(r_in.direction()), normalize(record.normal), refraction_index);
                scattered = ray(record.p, refract_direction);
                return true;
            }

        bool scatter_3(const ray& r_in, hit_record& record,
            color& attenuation, ray& scattered) const
            {
                attenuation = this->attenuation;
                record.mat_type = this->type;
                double ri = record.front_face ? refraction_index : (1.0f / refraction_index);

                auto cos_theta = dot(normalize(- r_in.direction()), normalize(record.normal));
                auto sin_theta = std::sqrt(1 - cos_theta * cos_theta);

                vec3 direction;
                if(ri * sin_theta > 1.0f)
                {
                    // Full Reflect
                    direction = reflect(r_in.direction(), record.normal);
                }
                else
                {
                    // View as Refract
                    direction = refract(r_in.direction(), record.normal, ri);
                }

                scattered = ray(record.p, direction);

                return true;
            }

        bool scatter(const ray& r_in, hit_record& record,
            color& attenuation, ray& scattered) const override
            {
                attenuation = this->attenuation;
                record.mat_type = this->type;
                double ri = record.front_face ? refraction_index : (1.0f / refraction_index);

                auto cos_theta = dot(normalize(- r_in.direction()), normalize(record.normal));
                auto sin_theta = std::sqrt(1 - cos_theta * cos_theta);

                bool cannot_refract = ri * sin_theta > 1.0f;
                vec3 direction;
                if(cannot_refract || reflectance(cos_theta, ri) > random_double())
                {
                    // Full Reflect
                    direction = reflect(r_in.direction(), record.normal);
                }
                else
                {
                    direction = refract(r_in.direction(), record.normal, ri);
                }

                // Update ray time
                // scattered = ray(record.p, direction);
                // scattered = ray(record.p, direction, r_in.time());
                // ------------------------------------------------
                scattered = ray(record.p, direction, r_in.get_time());


                return true;
            }

};