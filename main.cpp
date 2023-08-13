#define STBIW_ASSERT(x)
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "RTWeekend.h"

#include "PixelColor.h"
#include "Hittable.h"
#include "HittableList.h"
#include "Sphere.h"

#include "stb_image_write.h"
#include <iostream>

#define STBI_DISABLE_PNG_COMPRESSION stbi_write_png_compression_level = 0;

double hit_sphere(const Point3& center, double radius, const Ray& r) {
	Vec3 oc = r.origin - center;
	auto a = r.direction.lengthSquared();
	auto half_b = dot(oc, r.direction);
	auto c = dot(oc, oc) - radius * radius;
	auto discriminant = half_b * half_b - a * c;
	
	if (discriminant < 0) {
		return -1.0;
	}
	else {
		// distance to closest sphere intersection
		return (-half_b - sqrt(discriminant)) / a;
	}
}

Color rayColor(const Ray& r, const Hittable& world) {
	HitPoint rec;
	if (world.Hit(r, Interval(0, infinity), rec)) {
		return (rec.normal * 0.5) + 0.5;
	}

	Vec3 unit_direction = r.direction.normalized();
	double a = unit_direction.y() * 0.5 + 0.5;
	return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
}

int main()
{
	STBI_DISABLE_PNG_COMPRESSION

	int imageWidth = 1920;
	int imageHeight = 1080;

	double aspect_ratio = static_cast<double>(imageWidth) / imageHeight;
	
	double viewportHeight = 2.0;
	double viewportWidth = viewportHeight * aspect_ratio;

	Point3 cameraPosition = { 0, 0, 0 };
	Vec3 cameraForward = { 0, 0, 1 };

	double focalLength = 1;

	// Calculate the vectors across the horizontal and down the vertical viewport edges.
	Vec3 viewportU = Vec3(viewportWidth, 0, 0);
	Vec3 viewportV = Vec3(0, -viewportHeight, 0);

	// Calculate the horizontal and vertical delta vectors from pixel to pixel.
	Vec3 deltaU = viewportU / imageWidth;
	Vec3 deltaV = viewportV / imageHeight;

	// Calculate the location of the upper left pixel.
	Vec3 viewportTopLeft = cameraPosition - Vec3(0, 0, focalLength) - (viewportU / 2) - (viewportV / 2);
	Vec3 pixelTopLeft = viewportTopLeft + 0.5 * (deltaU + deltaV);

	// initialise output image
	PixelColor* image = new PixelColor[imageWidth * imageHeight * PixelColor::channels];

	HittableList world;

	world.add(make_shared<Sphere>(Point3(0, 0, -1), 0.5));
	world.add(make_shared<Sphere>(Point3(0, -100.5, -1), 100));

	// create image
	for (int y = 0; y < imageHeight; y++)
	{
		std::clog << "\rScanlines remaining: " << (imageHeight - y) << ' ' << std::flush;

		for (int x = 0; x < imageWidth; x++)
		{
			int pixelIndex = (x + y * imageWidth);

			Vec3 pixelPosition = pixelTopLeft + (x * deltaU) + (y * deltaV);
			Vec3 pixelDirection = pixelPosition - cameraPosition;

			Ray r(cameraPosition, pixelDirection);

			image[pixelIndex] = rayColor(r, world);
		}
	}
	
	// write image to file
	int pixelStride = sizeof(PixelColor);
	int rowStride = imageWidth * pixelStride;
	if (stbi_write_png("result.png", imageWidth, imageHeight, PixelColor::channels, image, rowStride) == 0) {
		// error
		return 1;
	}

	return 0;
}