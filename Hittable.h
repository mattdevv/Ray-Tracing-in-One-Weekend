#pragma once

#include "Ray.h"

struct HitPoint {
public:
	Point3 position;
	Vec3 normal;
	double t;
	bool isFrontFace;

	void set_face_normal(const Ray& r, const Vec3& outwardNormal) {
		// Sets the hit record normal vector.
		// NOTE: the parameter `outward_normal` is assumed to have unit length.

		isFrontFace = dot(r.direction, outwardNormal) < 0;
		normal = outwardNormal * (isFrontFace ? 1 : -1);
	}
};

class Hittable {
public:
	virtual ~Hittable() = default;

	virtual bool Hit(const Ray& r, Interval rayLengthLimits, HitPoint& record) const = 0;
};