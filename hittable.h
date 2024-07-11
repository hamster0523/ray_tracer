#pragma once

#include "rt.h"
#include <memory>

enum material_type
{
    LAMBERTIAN,
    METAL,
    DIELETRIC
};


class material;

// a hit point relative infomations
struct hit_record
{
    // hit point coord
    point3 p;
    // hit point normal
    vec3 normal;
    // hit point t : p = origin + t * direction
    double t;
    // whether the hit is object's front_face(outside)
    bool front_face;
    // hit point's material
    std::shared_ptr<material> mat;
    material_type mat_type;

    // Assume that the normal (point against) the ray
    void set_face_normal(const ray& r, const  vec3& outward_normal)
    {
        // Sets the hit record normal vector
        // The outward_normal is Assumed to a unit vector

        front_face = dot(normalize(r.direction()), outward_normal) < 0;

        normal = front_face ? outward_normal : - outward_normal;
    }

    void set_normal_y_axis_back()
    {
        normal.y() = - normal.y();
    }
};

class hittable
{
    public:
     virtual ~hittable() = default;

     virtual bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_record& record) const = 0;

     virtual bool hit(const ray& r, interval inter, hit_record& record) const = 0;
};