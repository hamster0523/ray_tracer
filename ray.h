#pragma once

#include "vec3.h"

class ray 
{
    private:
        point3 ori;
        vec3 dir;

    public:
        ray() {}

        ray(const point3& origin, const vec3& direction) : ori(origin), dir(direction) {}

        const point3& origin() const noexcept {
            return ori;
        }

        const vec3& direction() const noexcept
        {
            return dir;
        }

        point3 at(double t) const
        {
            return ori + t * dir;
        }


};