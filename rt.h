#pragma once

#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <utility>
#include <random>

// All need constant
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions
inline double degree_to_radians(double degrees)
{
    return (degrees / 180.0f) * pi;
}

template<typename T>
inline T lerp(const T& s1, const T& s2, double val)
{
    return val * s1 + (1 - val) * s2;
}

inline double random_double()
{
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

inline double random_double(double min, double max)
{
    return min + (max - min) * random_double();
}

// include common Headers
// Note : The Sequence of Header is important, Or not will not be able to solve dependencies problem

// 1. 
#include "vec3.h"
#include "interval.h"
#include "color.h"
#include "ray.h"

// 2.
#include "material.h"
#include "lambertian.h"
#include "metal.h"

// 3.
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"

// 4. render !
#include "camera.h"
