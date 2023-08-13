#pragma once

#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>


// Usings

using std::shared_ptr;
using std::make_shared;
using std::sqrt;

// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions

inline double Deg2Rad(double degrees) {
    return degrees * pi / 180.0;
}

inline double Rad2Deg(double radians) {
    return radians * 180.0 / pi;
}

inline double Random01() {
    // Returns a random real in [0,1).
    return rand() / (RAND_MAX + 1.0);
}

inline double RandomRange(double min, double max) {
    // Returns a random real in [min,max).
    return Random01() * (max - min) + min;
}

// Common Headers

#include "Interval.h"
#include "ray.h"
#include "vec3.h"