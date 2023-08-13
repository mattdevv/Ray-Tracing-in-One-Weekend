#pragma once

#include "PixelColor.h"

#include "stb_image_write.h"
#define STBI_DISABLE_PNG_COMPRESSION stbi_write_png_compression_level = 0;

class Texture {
public:

	Texture(int x, int y) : resolutionX(x), resolutionY(y), buffer(new PixelColor[x * y]) { }
	~Texture() { delete[] buffer; }

	void SetPixel(const int coordX, const int coordY, const PixelColor& color)
	{
		buffer[coordX + coordY * resolutionX] = color;
	}

	// returns function's success
	bool SaveToFile(char const* filename)
	{
		// write image to file
		int pixelStride = sizeof(PixelColor);
		int rowStride = resolutionX * pixelStride;
		if (stbi_write_png(filename, resolutionX, resolutionY, PixelColor::channels, buffer, rowStride) == 0) {
			return false; // error
		}

		return true;
	}

	int GetResolutionX() { return resolutionX; }
	int GetResolutionY() { return resolutionY; }
	double GetAspectRatio() { return static_cast<double>(resolutionX) / resolutionY; }

private:

	int resolutionX, resolutionY;
	PixelColor* buffer;
};