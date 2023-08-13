#pragma once

#include "Hittable.h"

#include <memory>
#include <vector>

using std::shared_ptr;
using std::make_shared;

class HittableList : public Hittable {
public:
	std::vector<shared_ptr<Hittable>> objects;


	HittableList() {}
	HittableList(shared_ptr<Hittable> object) { add(object); }

	void clear() { objects.clear(); }

	void add(shared_ptr<Hittable> object) {
		objects.push_back(object);
	}

    bool Hit(const Ray& r, Interval rayLengthLimits, HitPoint& rec) const override {
        HitPoint tempHit;
        bool hitSomething = false;
        double closest_so_far = rayLengthLimits.max;

        for (const auto& object : objects) {
            if (object->Hit(r, Interval(rayLengthLimits.min, closest_so_far), tempHit)) {
                hitSomething = true;
                closest_so_far = tempHit.t;
                rec = tempHit;
            }
        }

        return hitSomething;
    }
};