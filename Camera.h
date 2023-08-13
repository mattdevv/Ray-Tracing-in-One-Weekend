#pragma once

#include "RTWeekend.h"

#include "Hittable.h"
#include "Texture.h"
#include "PixelColor.h"


class Camera {
public:
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
				Vec3 pixelPosition = pixelTopLeft + (x * pixelDeltaU) + (y * pixelDeltaV);
				Vec3 pixelDirection = pixelPosition - position;

				Ray r(position, pixelDirection);

				outputTexture->SetPixel(x, y, RayColor(r, world));
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

	Color RayColor(const Ray& r, const Hittable& world) {
		HitPoint rec;
		if (world.Hit(r, Interval(0, infinity), rec)) {
			return (rec.normal * 0.5) + 0.5;
		}

		Vec3 unit_direction = r.direction.normalized();
		double a = unit_direction.y() * 0.5 + 0.5;
		return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
	}
};
