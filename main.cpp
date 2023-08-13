#define STBIW_ASSERT(x)
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <iostream>
#include <inttypes.h>
#include "PixelColor.h"
#include "stb_image_write.h"
#include "Ray.h"

#define STBI_DISABLE_PNG_COMPRESSION stbi_write_png_compression_level = 0;

double hit_sphere(const Point3& center, double radius, const Ray& r) {
	Vec3 oc = r.origin - center;
	auto a = dot(r.direction, r.direction);
	auto b = 2.0 * dot(oc, r.direction);
	auto c = dot(oc, oc) - radius * radius;
	auto discriminant = b * b - 4 * a * c;
	
	if (discriminant < 0) {
		return -1.0;
	}
	else {
		// distance to closest sphere intersection
		return (-b - sqrt(discriminant)) / (2.0 * a);
	}
}

Color rayColor(const Ray& r) {
	double sphereRadius = 0.5;
	Point3 spherePosition = Point3(0, 0, -1);

	double t = hit_sphere(spherePosition, sphereRadius, r);
	if (t > 0.0)
	{
		Vec3 N = (r.at(t) - spherePosition) / sphereRadius;
		return 0.5 * Color(N.x() + 1, N.y() + 1, N.z() + 1);
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

			image[pixelIndex] = rayColor(r);
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