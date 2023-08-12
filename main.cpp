#define STBIW_ASSERT(x)
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <iostream>
#include <inttypes.h>
#include "stb_image_write.h"

int main()
{
	int imageWidth = 256;
	int imageHeight = 256;
	int imageChannels = 4;

	// initialise output image
	uint8_t* image = new uint8_t[imageWidth * imageHeight * imageChannels];
	for (int i = 0; i < imageWidth * imageHeight * imageChannels; i++)
		image[i] = 255;

	// create image
	for (int y = 0; y < imageHeight; y++)
	{
		std::clog << "\rScanlines remaining: " << (imageHeight - y) << ' ' << std::flush;

		for (int x = 0; x < imageWidth; x++)
		{
			int pixelIndex = (x + y * imageHeight) * imageChannels;

			double u = x / static_cast<double>(imageWidth - 1);
			double v = y / static_cast<double>(imageHeight - 1);

			double r = u;
			double g = v;
			double b = 0;

			int ir = static_cast<int>(255.999 * r);
			int ig = static_cast<int>(255.999 * g);
			int ib = static_cast<int>(255.999 * b);

			image[pixelIndex + 0] = ir;
			image[pixelIndex + 1] = ig;
			image[pixelIndex + 2] = ib;
		}
	}

	// disable png compression
	stbi_write_png_compression_level = 0;

	// write image to file
	int pixelStride = imageChannels * sizeof(uint8_t);
	int rowStride = imageWidth * pixelStride;
	if (stbi_write_png("result.png", imageWidth, imageHeight, imageChannels, image, rowStride) == 0) {
		// error
		return 1;
	}

	return 0;
}