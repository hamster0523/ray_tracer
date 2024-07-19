#pragma once

#include <cmath>
#include <memory>
#include "rt.h"


class sphere : public hittable
{
    private:
        double rad;
        vec3 cent;
        std::shared_ptr<material> mat;
        bool is_moving;
        vec3 moving_vec;
        AABB bbox;

        void get_sphere_uv(const point3&p, double& u, double& v) const
        {
            // p : a given point on the sphere of r : 1, center at the (0, 0, 0)
            // u : returned value [0, 1] of angle around the Y axis from x = -1
            // v : returned value [0, 1] of angle from Y = -1 to Y = 1
            auto theta = std::acos(-p.y());
            auto phi   = std::atan2(-p.z(), p.x()) + pi;

            u = phi / (2 * pi);
            v = theta / pi;
        }

    public:
        sphere() : rad(1.0), cent(1.0) {
            mat = std::make_shared<lambertian>(color(0.4f, 0.4f, 0.4f));
        }

        sphere(double r, const vec3& vec) : rad(
            std::fmax(0, r)), cent(vec) {
                // TODO : Initialize the material pointer "mat"
                //        To indicate the specific material the sphere is
                // The Default material is lambertian
                mat = std::make_shared<lambertian>(color(0.4f, 0.4f, 0.4f));
            }
        
        // Stationary sphere
        template<typename material_type>
        sphere(double r, const vec3& vec, material_type&& mat) :
            rad(std::fmax(0, r)), cent(vec), mat(std::forward<material_type>(mat)), is_moving(false)
             {
                // Construct the AABB of the stationary sphere
                vec3 rvec = vec3(rad, rad, rad);
                bbox = AABB(cent - rvec, cent + rvec);
             }

        // Moving sphere
        template<typename material_type>
        sphere(double r, const vec3& center1, const vec3& center2, material_type&& mat) :
            rad(std::fmax(0, r)), cent(center1), mat(std::forward<material_type>(mat)), is_moving(true),
            moving_vec(center2 - center1) 
            {
                // Construct the AABB of the moving sphere
                auto rvec = vec3(rad, rad, rad);
                AABB bbox1(center1 - rvec, center1 + rvec);
                AABB bbox2(center2 - rvec, center2 + rvec);
                bbox = merge_two_AABB(bbox1, bbox2);
            }
  
        double radius() const noexcept
        {
            return rad;
        }

        const vec3& center() const noexcept
        {
            return cent;
        }

        double area() const noexcept
        {
            return pi * rad * rad;
        }

        bool hit_sphere(const ray& r) const noexcept
        {
            vec3 oc = cent - r.origin();
            auto a  = dot(r.direction(), r.direction());
            // never use variable b will be optimized out
            auto b  = -2.0f * dot(r.direction(), oc);
            auto h  = dot(r.direction(), oc);
            auto c  = dot(oc, oc) - rad * rad;
            // auto discrim = b * b - 4.0f * a * c;
            auto discrim = h * h - a * c;
            return discrim >= 0;
        }

        double hit_sphere_return_hit_point(const ray& r)
        {
            auto center = cent;
            auto radius = rad;

            vec3 oc = center - r.origin();
            auto a  = dot(r.direction(), r.direction());
            // never use variable b will be optimized out
            auto b  = -2.0f * dot(r.direction(), oc);
            auto h  = dot(r.direction(), oc);
            auto c  = dot(oc, oc) - radius * radius;
            // auto discrim = b * b - 4.0f * a * c;
            auto discrim = h * h - a * c;

            if(discrim < 0)
            {
                // the hit point is behind the camera
                return -100.0f;
            }
            else 
            {
                // else there is a hit point - return the nearest hit point
                // return (- b - std::sqrt(discrim)) / (2.0f * a);
                return (h - std::sqrt(discrim)) / a;
            }
        }

        bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_record& record) const override
        {
            // update the sphere center if it is moving
            // what time the ray is generated, that time is the ray hit the sphere
            vec3 sphere_center = is_moving ? this->sphere_center(r.get_time()) : cent;

            // vec3 oc = cent - r.origin();
            vec3 oc = sphere_center - r.origin();
            auto a  = r.direction().length_squared();
            auto h  = dot(r.direction(), oc);
            auto c  = oc.length_squared() - rad * rad;

            auto discriminator = h * h - a * c;
            if(discriminator < 0) return false;

            auto sqrt_discriminator = std::sqrt(discriminator);

            // Find the nearest root that lies in the acceptable range
            auto root = (h - sqrt_discriminator) / a;
            if(root <= ray_tmin || ray_tmax <= root)
            {
                root = (h + sqrt_discriminator) / a;
                if(root <= ray_tmin || ray_tmax <= root)
                {
                    return false;
                }
            }

            // For Sphere the t_min hit_point_coord hit_point_normal can be calculated as
            record.t = root;
            record.p = r.at(record.t);
            auto outside_normal = normalize(record.p - cent);
            // auto outside_normal = (record.p - cent) / rad;
            record.set_face_normal(r, outside_normal);
            // when hit -> update the hit point texture coord u and v
            get_sphere_uv(outside_normal, record.u, record.v);
            record.mat = mat;
            //record.normal = outside_normal;
            return true;
        }

        bool hit(const ray& r, interval inter, hit_record& record) const override
        {
            // update the sphere center if it is moving
            // what time the ray is generated, that time is the ray hit the sphere
            vec3 sphere_center = is_moving ? this->sphere_center(r.get_time()) : cent;

            // vec3 oc = cent - r.origin();
            vec3 oc = sphere_center - r.origin();
            auto a  = r.direction().length_squared();
            auto h  = dot(r.direction(), oc);
            auto c  = oc.length_squared() - rad * rad;

            auto discriminator = h * h - a * c;
            if(discriminator < 0) return false;

            auto sqrt_discriminator = std::sqrt(discriminator);

            // Find the nearest root that lies in the acceptable range
            auto root = (h - sqrt_discriminator) / a;
            // inter.surround_noequ(root) => root < tmax && root > tmin
            // !inter.surround_noequ(root) => root >= tmax && root <= tmin
            if(! inter.surround_noequ(root))
            {
                root = (h + sqrt_discriminator) / a;
                if(! inter.surround_noequ(root))
                {
                    return false;
                }
            }

            // For Sphere the t_min hit_point_coord hit_point_normal can be calculated as
            record.t = root;
            record.p = r.at(record.t);
            auto outside_normal = normalize(record.p - cent);
            // auto outside_normal = (record.p - cent) / rad;
            record.set_face_normal(r, outside_normal);
            // When Hit -> Update the hit point texture coord u and v
            get_sphere_uv(outside_normal, record.u, record.v);
            record.mat = mat;
            //record.normal = outside_normal;
            return true;
        }

        point3 sphere_center(double t) const
        {
            // the sphere is moving with a constant velocity
            // Linearly interpolate the sphere center between the start and end points
            if(is_moving)
            {
                return cent + t * moving_vec;
            }
            else
            {
                return cent;
            }
        }

        AABB bounding_box() const override
        {
            return bbox;
        }
};