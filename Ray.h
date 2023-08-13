#pragma once

#include "Vec3.h"

class Ray {
public:
	Point3 origin;
	Vec3 direction;

	Ray() {}

	Ray(const Point3& origin, const Vec3& direction) : origin(origin), direction(direction) {}

	Point3 at(double t) const {
		return origin + t * direction;
	}
};