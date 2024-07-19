#pragma once

#include "rt.h"

class AABB
{
    public:
        interval x_interval, y_interval, z_interval;
    
        AABB() {}

        AABB(const interval& x, const interval& y, const interval& z) : x_interval(x), y_interval(y), z_interval(z) {}

        // Extreme Case : Two points to construct a AABB
        AABB(const point3& a, const point3& b)
        {
            x_interval = (a[0] <= b[0]) ? interval(a[0], b[0]) : interval(b[0], a[0]);
            y_interval = (a[1] <= b[1]) ? interval(a[1], b[1]) : interval(b[1], a[1]);
            z_interval = (a[2] <= b[2]) ? interval(a[2], b[2]) : interval(b[2], a[2]);
        }

        // Get Axis interval
        const interval& axis_interval(int axis) const
        {
            if(axis == 0) return x_interval;
            if(axis == 1) return y_interval;
            if(axis == 2) return z_interval;
            std::runtime_error("Invalid axis");
            // Unreachable
            __builtin_unreachable();
        }

        // Ray hit with AABB
        bool hit(const ray& r, interval record_overlap) const
        {
            const point3& ray_origin = r.origin();
            const vec3&   ray_dir    = r.direction();

            for(int axis = 0; axis < 3 ; axis ++)
            {
                auto axis_interval = this->axis_interval(axis);

                auto denominator = 1.0f / ray_dir[axis];

                auto t0 = (axis_interval.min - ray_origin[axis]) * denominator;
                auto t1 = (axis_interval.max - ray_origin[axis]) * denominator;

                if(t0 < t1)
                {
                    record_overlap.min = std::fmax(t0, record_overlap.min);
                    record_overlap.max = std::fmin(t1, record_overlap.max);
                }
                else
                {
                    record_overlap.min = std::fmax(t1, record_overlap.min);
                    record_overlap.max = std::fmin(t0, record_overlap.max);
                }

                if(record_overlap.max <= record_overlap.min) return false;
            }

            return true;
        }

        // Merge Two AABB
        AABB (const AABB& a, const AABB& b)
        {
            x_interval = two_interval_intersection(a.x_interval, b.x_interval);
            y_interval = two_interval_intersection(a.y_interval, b.y_interval);
            z_interval = two_interval_intersection(a.z_interval, b.z_interval);
        }

        int longest_axis() const noexcept
        {
            if(x_interval.size() > y_interval.size())
                return x_interval.size() > z_interval.size() ? 0 : 2;
            else
                return y_interval.size() > z_interval.size() ? 1 : 2;
        }

        double area() const noexcept
        {
            return x_interval.size() * y_interval.size() * z_interval.size();
        }
        
        // Empty AABB and Universe AABB
        static const AABB empty, universe;
};

const AABB AABB::empty = AABB(interval::empty, interval::empty, interval::empty);
const AABB AABB::universe = AABB(interval::universe, interval::universe, interval::universe);

inline AABB merge_two_AABB(const AABB& a, const AABB& b)
{
    auto x_interval = two_interval_intersection(a.x_interval, b.x_interval);
    auto y_interval = two_interval_intersection(a.y_interval, b.y_interval);
    auto z_interval = two_interval_intersection(a.z_interval, b.z_interval);
    return AABB(x_interval, y_interval, z_interval);
}