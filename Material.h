#pragma once

#include "RTWeekend.h"

class HitPoint;

class Material {
public:

	virtual ~Material() = default;

	virtual bool scatter(const Ray& inRay, const HitPoint& rec, Color& attenuation, Ray& outRay) const = 0;
};

class Lambertian : public Material {
public:

	Lambertian(const Color& a) : albedo(a) { }

	bool scatter(const Ray& inRay, const HitPoint& rec, Color& attenuation, Ray& outRay) const override {
		Vec3 scatterDirection = rec.normal + RandomPointOnUnitSphere();

		// Catch degenerate scatter direction
		if (scatterDirection.isNearZeroLength())
			scatterDirection = rec.normal;

		outRay = Ray(rec.position, scatterDirection);
		attenuation = albedo;
		return true;
	}

private:
	Color albedo;
};

class Metal : public Material {
public:
	Metal(const Color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

	bool scatter(const Ray& inRay, const HitPoint& rec, Color& attenuation, Ray& outRay) const override {
		Vec3 reflected = reflect(inRay.direction.normalized(), rec.normal);
		outRay = Ray(rec.position, reflected + fuzz * RandomPointOnUnitSphere());
		attenuation = albedo;

		// check if outgoing ray gets absorbed by surface
		return (dot(outRay.direction, rec.normal) > 0);
	}

private:
	Color albedo;
	double fuzz;
};

class Dielectric : public Material {
public:
	Dielectric(double _IOR) : IOR(_IOR) {}

	bool scatter(const Ray& inRay, const HitPoint& rec, Color& attenuation, Ray& outRay) const override {
		attenuation = Color(1, 1, 1);
		double refractionRatio = rec.isFrontFace ? (1.0 / IOR) : IOR;

		Vec3 unitRayDir = inRay.direction.normalized();
		Vec3 refractedDir = refract(unitRayDir, rec.normal, refractionRatio);

		outRay = Ray(rec.position, refractedDir);
		return true;
	}

private:
	double IOR; // index of refraction
};