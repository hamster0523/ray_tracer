#pragma once

#include "vec3.h"

class ray 
{
    private:
        point3 ori;
        vec3 dir;
        double time;

    public:
        ray() : ori(point3(0, 0, 0)), dir(vec3(0, 0, 0)), time(0) {}

        ray(const point3& origin, const vec3& direction) : ori(origin), dir(direction), time(0) {}

        ray(const point3& origin, const vec3& direction, double t) : ori(origin), 
            dir(direction), time(t) {}

        const point3& origin() const noexcept {
            return ori;
        }

        const vec3& direction() const noexcept
        {
            return dir;
        }

        double get_time() const noexcept
        {
            return time;
        }

        point3 at(double t) const
        {
            return ori + t * dir;
        }


};