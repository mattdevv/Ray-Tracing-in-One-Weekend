#pragma once

#include "Hittable.h"
#include "Vec3.h"

class Sphere : public Hittable {
public:
	Sphere(Point3 _center, double _radius, shared_ptr<Material> _material) 
		: center(_center), radius(_radius), mat(_material) {}

	bool Hit(const Ray& r, Interval rayLengthLimits, HitPoint& record) const override {
		Vec3 oc = r.origin - center;
		double a = r.direction.lengthSquared();
		double half_b = dot(oc, r.direction);
		double c = dot(oc, oc) - radius * radius;

		double discriminant = half_b * half_b - a * c;
		if (discriminant < 0) {
			return false;
		}
		double sqrtd = sqrt(discriminant);

		// Find the nearest root that lies in the acceptable range.
		double root = (-half_b - sqrtd) / a;
		if (!rayLengthLimits.surrounds(root)) {
			// check second root
			root = (-half_b + sqrtd) / a; 
			if (!rayLengthLimits.surrounds(root)) {
				return false;
			}
		}

		record.t = root;
		record.position = r.at(record.t);
		Vec3 outward_normal = (record.position - center) / radius;
		record.set_face_normal(r, outward_normal);
		record.mat = mat;

		return true;
	}

private:
	Point3 center;
	double radius;
	shared_ptr<Material> mat;
};