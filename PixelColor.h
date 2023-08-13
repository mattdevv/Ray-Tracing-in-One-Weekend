#pragma once

#include <inttypes.h>
#include "Vec3.h"

class PixelColor {
public:
	static const int channels = 4;

	uint8_t rgba[channels];

	PixelColor() : rgba{ 0, 0, 0, 0 } {}

	PixelColor(Color color, double a = 1.0) : PixelColor(color[0], color[1], color[2], a) {}

	PixelColor(double r, double g, double b, double a = 1.0) : rgba{
		static_cast<uint8_t>(255.999 * r), 
		static_cast<uint8_t>(255.999 * g),
		static_cast<uint8_t>(255.999 * b),
		static_cast<uint8_t>(255.999 * a)
	} {}
};

static_assert(sizeof(PixelColor) == PixelColor::channels, "Color struct incorrect size");