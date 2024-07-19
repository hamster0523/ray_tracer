#pragma once

#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <utility>
#include <random>
#include <algorithm>
#include <functional>
#include <tuple>
#include <chrono>

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

template<typename T>
inline T clamp(T val, T min, T max)
{
    if(val < min) return min;
    if(val > max) return max;
    return val;
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

inline int random_int(int min, int max)
{
    return int(random_double(min, max + 1));
}

double reflectance(double cosine, double refraction_index)
{
    // Schilick approximation
    double r0 = (1 - refraction_index) / (1 + refraction_index);
    r0 = r0 * r0;
    return r0 + (1 - r0) * std::pow((1 - cosine), 5);
}

// include common Headers
// Note : The Sequence of Header is important, Or not will not be able to solve dependencies problem

// 1. 
#include "rt_stb_image.h"
#include "vec3.h"
#include "interval.h"
#include "color.h"
#include "ray.h"
#include "aabb.h"

// 2. 
#include "texture.h"
#include "solidcolor.h"
#include "checkTexture.h"
#include "ImageTexture.h"

// 3.
#include "material.h"
#include "lambertian.h"
#include "metal.h"
#include "dielectric.h"

// 4.
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
#include "bvh.h"

// 5. render !
#include "camera.h"
