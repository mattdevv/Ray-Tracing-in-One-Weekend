#define STBIW_ASSERT(x)
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <iostream>
#include <inttypes.h>
#include "PixelColor.h"
#include "stb_image_write.h"
#include "Ray.h"

#define STBI_DISABLE_PNG_COMPRESSION stbi_write_png_compression_level = 0;

Color rayColor(const Ray& r) {
	return Color(0, 0, 0);
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
	Vec3 viewport_upper_left = cameraPosition - Vec3(0, 0, focalLength) - (viewportU / 2) - (viewportV / 2);
	Vec3 pixel00_loc = viewport_upper_left + 0.5 * (deltaU + deltaV);

	// initialise output image
	PixelColor* image = new PixelColor[imageWidth * imageHeight * PixelColor::channels];

	// create image
	for (int y = 0; y < imageHeight; y++)
	{
		std::clog << "\rScanlines remaining: " << (imageHeight - y) << ' ' << std::flush;

		for (int x = 0; x < imageWidth; x++)
		{
			int pixelIndex = (x + y * imageWidth);

			Vec3 pixelPosition = pixel00_loc + (x * deltaU) + (y * deltaV);
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