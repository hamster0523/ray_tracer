#pragma once

#include "rt.h"
#include "hittable.h"

using albedo = color;

class material
{
    public:
        virtual ~material() = default;

        virtual bool scatter(
            const ray& r_in, const hit_record& record,
            color& attenuation, ray& scattered
        ) const 
        {
            return false;
        }
};