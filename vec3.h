#pragma once

#include <cmath>
#include <iostream>

class vec3
{
    public:
        double e[3];

    public:
        vec3() : e{0,0,0} {}
        explicit vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}
        explicit vec3(double val) : e{val, val, val} {}

        // double x() const noexcept { return e[0]; }
        // double y() const noexcept { return e[1]; }
        // double z() const noexcept { return e[2]; }

        vec3 operator-() const noexcept {
            return vec3(-e[0], -e[1], -e[2]);
        }

        double operator[](size_t i) const noexcept {
            return e[i];
        }

        double& operator[](size_t i) noexcept
        {
            return e[i];
        }

        double x() const noexcept
        {
            return e[0];
        }

        double& x() noexcept
        {
            return e[0];
        }

        double y() const noexcept
        {
            return e[1];
        }

        double& y() noexcept
        {
            return e[1];
        }

        double z() const noexcept
        {
            return e[2];
        }

        double& z() noexcept
        {
            return e[2];
        }

        vec3 operator*(double t) noexcept
        {
            return vec3{e[0] * t, e[1] * t, e[2] * t};
        }

        vec3& operator+=(const vec3& v) noexcept
        {
            e[0] += v.e[0];
            e[1] += v.e[1];
            e[2] += v.e[2];
            return *this;
        }

        vec3& operator*=(double t) noexcept
        {
            e[0] *= t;
            e[1] *= t;
            e[2] *= t;
            return *this;
        }

        vec3& operator/=(double t) noexcept
        {
            return *this *= 1 / t;
        }

        double length_squared() const noexcept
        {
            return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
        }

        double length() const noexcept
        {
            return std::sqrt(length_squared());
        }

        // vec3 normalize() noexcept const
        // {
        //     return vec3{e[0], e[1], e[2]} / length();
        // }
        static vec3 random()
        {
            return vec3(
                random_double(),
                random_double(),
                random_double()
            );
        }

        static vec3 random(
            double min, double max
        )
        {
            return vec3(
                random_double(min, max),
                random_double(min, max),
                random_double(min, max)
            );
        }

        bool near_zero() const
        {
            // Return true if the vector is close to zero
            static auto thresh = 1e-8;
            return (std::fabs(e[0]) < thresh &&
                    std::fabs(e[1]) < thresh &&
                    std::fabs(e[2]) < thresh);
        }

    friend std::ostream& operator<<(std::ostream&out, const vec3& vec);
    friend vec3 operator*(double t, const vec3& vec);
};

// a alias type name for vec3
using point3 = vec3;

inline std::ostream& operator<<(std::ostream& out, const vec3& vec)
{
    out << vec.e[0] << " " << vec.e[1] << " " << vec.e[2];
    return out;
}

inline vec3 operator+(const vec3& u, const vec3& v)
{
    return vec3{u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]};
}

inline vec3 operator-(const vec3& u, const vec3& v)
{
    return vec3{u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]};
}

inline vec3 operator*(const vec3& u, const vec3& v)
{
    return vec3{u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]};
}

inline vec3 operator*(double t, const vec3& v)
{
    return vec3{v.e[0] * t, v.e[1] * t, v.e[2] * t};
}

inline vec3 operator/(const vec3& v, double t)
{
    return ( 1 / t ) * v;
}

inline double dot(const vec3& u, const vec3& v)
{
    return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}

inline vec3 cross(const vec3& u, const vec3& v)
{
    vec3 res;
    res.e[0] = u.e[1] * v.e[2] - u.e[2] * v.e[1];
    res.e[1] = u.e[2] * v.e[0] - u.e[0] * v.e[2];
    res.e[2] = u.e[0] * v.e[1] - u.e[1] * v.e[0];
    return res;
}

inline vec3 normalize(const vec3& v)
{
    return v / v.length();
}

inline vec3 unit_vector(const vec3& v)
{
    return normalize(v);
}

inline vec3 random_in_unit_sphere()
{
    while(true)
    {
        auto p = vec3::random(-1, 1);
        if(p.length_squared() < 1)
            return p;
    }
}

inline vec3 random_unit_vector()
{
    return normalize(random_in_unit_sphere());
}

inline vec3 random_on_hemisphere(const vec3& normal)
{
    vec3 on_unit_sphere_vec3 = random_unit_vector();
    if(dot(normal, on_unit_sphere_vec3) > 0.0f)
        // Same Side
        return on_unit_sphere_vec3;
    else
        return - on_unit_sphere_vec3;
}

inline vec3 my_reflect(const vec3& v, const vec3& n)
{
    auto this_v = v;
    auto this_n = n;
    if(dot(normalize(this_n), normalize(this_v)) > 0.0f)
    {
        this_v = - v;
    }
    // https://raytracing.github.io/images/fig-1.15-reflection.jpg
    // cos_theta
    auto cos_theta = dot(normalize(this_n), normalize(this_v));
    // theta + phi = pi
    // theta = pi - phi
    // cos_theta = cos(pi - phi) = cos(pi)cos(phi) + sin(pi)sin(phi) = - cos(phi)
    // cos(phi) = - cos_theta
    auto cos_phi = - cos_theta;
    auto b = - this_v * cos_phi;
    return this_v + 2 * b;
}

inline vec3 reflect(const vec3& v, const vec3& n)
{
    // https://www.cnblogs.com/graphics/archive/2013/02/21/2920627.html
    return v - 2 * dot(v, n) * n;
}

inline vec3 refract(const vec3& R_in, const vec3& normal, double param)
{
    // R_in is the unit vector from outside to point the hit point
    // normal is the hit point normal
    // param is the outside_refract_param / inside_refract_param

    auto normalized_Rin = normalize(R_in);
    auto normalized_normal = normalize(normal);

    auto cos_theta = std::fmin(dot(normalized_Rin, normalized_normal), 1.0f);
    auto R_perp = param * (normalized_Rin + cos_theta * normalized_normal);

    auto R_prep_lenthg_squared = R_perp.length_squared();
    auto R_parallel = vec3 {
        - std::sqrt(1.0f - R_prep_lenthg_squared * normalized_normal.x()),
        - std::sqrt(1.0f - R_prep_lenthg_squared * normalized_normal.y()),
        - std::sqrt(1.0f - R_prep_lenthg_squared * normalized_normal.z())
    };

    return R_parallel + R_perp;
}

inline vec3 refract2(const vec3& uv, const vec3& n, double etai_over_etat)
{
    auto cos_theta = std::fmin(1.0, dot(-uv, n));
    vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
    vec3 r_out_parallel = - std::sqrt(std::fabs(1.0f - r_out_perp.length_squared())) * n;
    return r_out_perp + r_out_parallel;
}