#define STBIW_ASSERT(x)
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <iostream>
#include <inttypes.h>
#include "color.h"
#include "stb_image_write.h"

#define STBI_DISABLE_PNG_COMPRESSION stbi_write_png_compression_level = 0;

int main()
{
	STBI_DISABLE_PNG_COMPRESSION

	int imageWidth = 256;
	int imageHeight = 256;

	// initialise output image
	Color* image = new Color[imageWidth * imageHeight * Color::channels];

	// create image
	for (int y = 0; y < imageHeight; y++)
	{
		std::clog << "\rScanlines remaining: " << (imageHeight - y) << ' ' << std::flush;

		for (int x = 0; x < imageWidth; x++)
		{
			int pixelIndex = (x + y * imageHeight);

			double u = x / static_cast<double>(imageWidth - 1);
			double v = y / static_cast<double>(imageHeight - 1);

			double r = u;
			double g = v;
			double b = 0;

			image[pixelIndex] = Color(r, g, b);
		}
	}
	
	// write image to file
	int pixelStride = sizeof(Color);
	int rowStride = imageWidth * pixelStride;
	if (stbi_write_png("result.png", imageWidth, imageHeight, Color::channels, image, rowStride) == 0) {
		// error
		return 1;
	}

	return 0;
}