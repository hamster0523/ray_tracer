#pragma once

#include "vec3.h"
#include "interval.h"
#include <iostream>

using color = vec3;
static double gamma_corelation = 1.0f / 2.2f;

inline double linear_to_gamma(double linear_component)
{
    if(linear_component > 0)
        return std::pow(linear_component, gamma_corelation);
    return 0;
}

void write_color(std::ostream& out, const color& color)
{
    auto r = color.x();
    auto g = color.y();
    auto b = color.z();

    r = linear_to_gamma(r);
    g = linear_to_gamma(g);
    b = linear_to_gamma(b);

    static const interval color_intensity(0.000, 0.999);
    int rbytes = static_cast<int>(256 * color_intensity.clamp(r));
    int gbytes = static_cast<int>(256 * color_intensity.clamp(g));
    int bbytes = static_cast<int>(256 * color_intensity.clamp(b));

    out << rbytes << " " << gbytes << " " << bbytes << "\n";
}
