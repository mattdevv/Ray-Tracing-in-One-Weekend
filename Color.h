#pragma once

#include <inttypes.h>

class Color {
public:
	static const int channels = 4;

	uint8_t rgba[channels];

	Color() : rgba{ 0, 0, 0, 0 } {}

	Color(double r, double g, double b, double a = 1.0) : rgba{
		static_cast<uint8_t>(255.999 * r), 
		static_cast<uint8_t>(255.999 * g),
		static_cast<uint8_t>(255.999 * b),
		static_cast<uint8_t>(255.999 * a)
	} {}
};

static_assert(sizeof(Color) == Color::channels, "Color struct incorrect size");