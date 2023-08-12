#define STBIW_ASSERT(x)
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <inttypes.h>
#include "stb_image_write.h"

int main()
{
	int imageWidth = 256;
	int imageHeight = 256;
	int imageChannels = 4;

	uint8_t* image = new uint8_t[imageWidth * imageHeight * imageChannels];

	for (int i = 0; i < imageWidth * imageHeight * imageChannels; i++)
		image[i] = 255;

	// disable png compression
	stbi_write_png_compression_level = 0;

	// write image to file
	int pixelStride = imageChannels * sizeof(uint8_t);
	int rowStride = imageWidth * pixelStride;
	if (stbi_write_png("result.png", imageWidth, imageHeight, imageChannels, image, rowStride) == 0)
		return 1;

	return 0;
}