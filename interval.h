#pragma once


class interval
{
    public:
        double min, max;
        
        interval() : min(infinity), max(- infinity) {}

        interval(double min, double max) : min(min), max(max) {}

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

        static const interval empty, universe;
};

const interval interval::empty = interval(+ infinity, - infinity);

const interval interval::universe = interval(- infinity, + infinity);