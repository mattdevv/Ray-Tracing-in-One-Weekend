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
	Dielectric(double _IOR) : IOR(_IOR), abspCoeff(0,0,0) {}
	Dielectric(double _IOR, const Color& absorption) : IOR(_IOR), abspCoeff(absorption) {}

	bool scatter(const Ray& inRay, const HitPoint& rec, Color& attenuation, Ray& outRay) const override {
		if (rec.isFrontFace) {
			// no absorption
			attenuation = Color(1, 1, 1);
		}
		else {
			// Beer-Lambert absorption occurs over ray's length
			double rayLength = (inRay.origin - rec.position).length();
			attenuation = absorption(rayLength);
		}
		
		double refractionRatio = rec.isFrontFace ? (1.0 / IOR) : IOR;

		Vec3 unitRayDir = inRay.direction.normalized();
		double cosTheta = fmin(dot(-unitRayDir, rec.normal), 1.0);
		double sinTheta = sqrt(1.0 - cosTheta * cosTheta);

		bool cannot_refract = refractionRatio * sinTheta > 1.0;
		Vec3 outDirection;

		if (cannot_refract || reflectance(cosTheta, refractionRatio) > Random01())
			outDirection = reflect(unitRayDir, rec.normal);
		else 
			outDirection = refract(unitRayDir, rec.normal, refractionRatio);

		outRay = Ray(rec.position, outDirection);
		return true;
	}

private:
	double IOR; // index of refraction
	Color abspCoeff;

	Color absorption(double rayLength) const {
		double R = exp(-abspCoeff.e[0] * rayLength);
		double G = exp(-abspCoeff.e[1] * rayLength);
		double B = exp(-abspCoeff.e[2] * rayLength);

		return  Color(R, G, B);
	}

	static double reflectance(double cosine, double ref_idx) {
		// Use Schlick's approximation for reflectance.
		auto r0 = (1 - ref_idx) / (1 + ref_idx);
		r0 = r0 * r0;
		return r0 + (1 - r0) * pow((1 - cosine), 5);
	}
};