#pragma once


class interval
{
    public:
        double min, max;
        
        interval() : min(infinity), max(- infinity) {}

        interval(double min, double max) : min(min), max(max) {}

        // Input two interval and return the intersection
        interval(const interval& a, const interval& b)
        {
            min = a.min <= b.min ? a.min : b.min;
            max = a.max >= b.max ? a.max : b.max;
        }
        
        double size() const noexcept
        {
            return max - min;
        }

        bool contains_withequ(double val) const noexcept
        {
            return val >= min && val <= max;
        }

        bool surround_noequ(double val) const noexcept
        {
            return val > min && val < max;
        }

        double clamp(double x) const noexcept
        {
            if(x < min) return min;
            if(x > max) return max;
            return x;
        }

        double interval_center() const noexcept
        {
            return (min + max) / 2.0f;
        }

        // interval padding deal with the precision issue
        interval expand(double delta) const noexcept
        {
            auto padding = delta / 2.0;
            return interval(min - padding, max + padding);
        }

        static const interval empty, universe;
};

const interval interval::empty = interval(+ infinity, - infinity);

const interval interval::universe = interval(- infinity, + infinity);

// Two interval instersection
inline interval two_interval_intersection(const interval& a, const interval& b)
{
    auto new_min = a.min <= b.min ? a.min : b.min;
    auto new_max = a.max >= b.max ? a.max : b.max;
    return interval(new_min, new_max);
}

inline interval two_interval_merge(const interval& a, const interval& b)
{
    auto new_min = std::fmin(a.min, b.min);
    auto new_max = std::fmax(a.max, b.max);
    return interval(new_min, new_max);
}