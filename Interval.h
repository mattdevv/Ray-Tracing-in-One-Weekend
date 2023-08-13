#pragma once

#include "Vec3.h"

class Interval {
public:
	double min, max;

	Interval() : min(+infinity), max(-infinity) {} // Default interval is empty
    Interval(double _min, double _max) : min(_min), max(_max) {}

    bool contains(double x) const {
        return min <= x && x <= max;
    }

    bool surrounds(double x) const {
        return min < x && x < max;
    }

    double clamp(double x) const {
        if (x <= min) return min;
        if (x >= max) return max;
        else          return x;
    }

    Vec3 clamp(const Vec3& x) const {
        return {
            clamp(x.e[0]),
            clamp(x.e[1]),
            clamp(x.e[2])
        };
    }

    static const Interval empty, universe;
};

const static Interval empty(+infinity, -infinity);
const static Interval universe(-infinity, +infinity);
