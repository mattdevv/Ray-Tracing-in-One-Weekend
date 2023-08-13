#pragma once

#include "RTWeekend.h"

#include "Hittable.h"
#include "Texture.h"
#include "PixelColor.h"

class Material;

class Camera {
public:
	int samplesPerPixel = 10;
	int maxRayBounces = 10;   // Maximum number of ray bounces into scene

	Camera(shared_ptr<Texture> _outputTexture) : outputTexture(_outputTexture) { }

	void Render(const Hittable& world) {
		Initialize(Point3(0, 0, 0));

		int maxX = outputTexture->GetResolutionX();
		int maxY = outputTexture->GetResolutionY();

		for (int y = 0; y < maxY; y++)
		{
			std::clog << "\rScanlines remaining: " << (maxY - y) << ' ' << std::flush;

			for (int x = 0; x < maxX; x++)
			{
				Color resultColor(0, 0, 0);

				for (int i = 0; i < samplesPerPixel; i++)
				{
					Ray r = GetRay(x, y);
					resultColor += RayColor(r, maxRayBounces, world);
				}

				// average samples
				resultColor /= static_cast<double>(samplesPerPixel);
				// clamp color gammut
				resultColor = Interval(0, 1).clamp(resultColor);
				// linear to gamma color conversion
				double gamma = 1.0 / 2.0;
				resultColor.e[0] = pow(resultColor.e[0], gamma);
				resultColor.e[1] = pow(resultColor.e[1], gamma);
				resultColor.e[2] = pow(resultColor.e[2], gamma);

				outputTexture->SetPixel(x, y, resultColor);
			}
		}

		std::clog << "\rDone.                 \n";
	}

private:

	Point3 position;
	Point3 pixelTopLeft;
	Vec3 pixelDeltaU;
	Vec3 pixelDeltaV;

	shared_ptr<Texture> outputTexture;

	void Initialize(const Point3& cameraPosition) {
		double imageWidth = static_cast<double>(outputTexture->GetResolutionX());
		double imageHeight = static_cast<double>(outputTexture->GetResolutionY());

		position = cameraPosition;
		auto focalLength = 1.0;

		// Determine viewport dimensions.
		double aspect_ratio = imageWidth / imageHeight;
		double viewportHeight = 2.0;
		double viewportWidth = viewportHeight * aspect_ratio;

		// Calculate the vectors across the horizontal and down the vertical viewport edges.
		Vec3 viewportU = Vec3(viewportWidth, 0, 0);
		Vec3 viewportV = Vec3(0, -viewportHeight, 0);

		// Calculate the horizontal and vertical delta vectors from pixel to pixel.
		pixelDeltaU = viewportU / imageWidth;
		pixelDeltaV = viewportV / imageHeight;

		// Calculate the location of the upper left pixel.
		Vec3 viewportTopLeft = position - Vec3(0, 0, focalLength) - (viewportU / 2) - (viewportV / 2);
		pixelTopLeft = viewportTopLeft + 0.5 * (pixelDeltaU + pixelDeltaV);
	}

	Ray GetRay(int i, int j) const {
		// Get a randomly sampled camera ray for the pixel at location i,j.

		Vec3 pixelCenter = pixelTopLeft + (i * pixelDeltaU) + (j * pixelDeltaV);
		Vec3 pixelSample = pixelCenter + pixelRandomOffset();

		Point3 rayOrigin = position;
		Vec3 rayDirection = pixelSample - rayOrigin;

		return Ray(rayOrigin, rayDirection);
	}

	Vec3 pixelRandomOffset() const {
		// Returns a random offset within a pixel square
		double px = RandomRange(-0.5, 0.5);
		double py = RandomRange(-0.5, 0.5);
		return (px * pixelDeltaU) + (py * pixelDeltaV);
	}

	Color RayColor(const Ray& r, int depth, const Hittable& world) {
		HitPoint rec;

		// If we've exceeded the ray bounce limit, no more light is gathered.
		if (depth <= 0)
			return Color(0, 0, 0);

		if (world.Hit(r, Interval(0.001, infinity), rec)) {
			Ray outScatteredRay;
			Color attenuation;

			if (rec.mat->scatter(r, rec, attenuation, outScatteredRay)) {
				return attenuation * RayColor(outScatteredRay, depth - 1, world);
			}

			return Color(0, 0, 0);
		}

		Vec3 unit_direction = r.direction.normalized();
		double a = unit_direction.y() * 0.5 + 0.5;
		return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
	}
};
