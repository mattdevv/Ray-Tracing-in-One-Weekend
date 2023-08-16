#pragma once

#include "RTWeekend.h"

#include "Hittable.h"
#include "Texture.h"
#include "PixelColor.h"

#include <chrono>

using namespace std::chrono;

class Material;

std::string NanoToHHMMSS(nanoseconds time)
{
	const auto hrs = duration_cast<hours>(time);
	const auto mins = duration_cast<minutes>(time - hrs);
	const auto secs = duration_cast<seconds>(time - hrs - mins);

	int hrs_i = hrs.count();
	int mins_i = mins.count();
	int secs_i = secs.count();

	if (hrs_i >= 100)
		return "Err: OoS";

	std::string output(8, '\0');

	output[0] = '0' + (hrs_i / 10);
	output[1] = '0' + (hrs_i % 10);
	output[2] = ':';

	output[3] = '0' + (mins_i / 10);
	output[4] = '0' + (mins_i % 10);
	output[5] = ':';

	output[6] = '0' + (secs_i / 10);
	output[7] = '0' + (secs_i % 10);

	return output;
}

class Camera {
public:
	int samplesPerPixel = 10;				// Maximum number of light samples per pixel
	int maxRayBounces = 10;					// Maximum number of ray bounces per sample

	double vfov = 90;						// Vertical view angle (field of view)
	Point3 lookfrom = Point3(0, 0, -1);		// Point camera is looking from
	Point3 lookat = Point3(0, 0, 0);		// Point camera is looking at
	Vec3   vup = Vec3(0, 1, 0);				// Camera-relative "up" direction

	double defocusAngle = 0;				// Variation angle of rays through each pixel
	double focusDist = 10;					// Distance from camera lookfrom point to plane of perfect focus

	Camera(shared_ptr<Texture> _outputTexture) : outputTexture(_outputTexture) { }

	void Render(const Hittable& world) {
		Initialize();

		int maxX = outputTexture->GetResolutionX();
		int maxY = outputTexture->GetResolutionY();

		nanoseconds totalTime_ns = nanoseconds::zero();

		for (int y = 0; y < maxY; y++)
		{
			high_resolution_clock::time_point t_start = high_resolution_clock::now();

			for (int x = 0; x < maxX; x++)
			{
				Color resultColor(0, 0, 0);

				for (int i = 0; i < samplesPerPixel; i++)
				{
					Ray r = GetRay(x, y);
					resultColor += RayColor(r, maxRayBounces, world);
				}

				// average samples
				resultColor /= static_cast<double>(samplesPerPixel);
				// clamp color gammut
				resultColor = Interval(0, 1).clamp(resultColor);
				// linear to gamma color conversion
				double gamma = 1.0 / 2.0;
				resultColor.e[0] = pow(resultColor.e[0], gamma);
				resultColor.e[1] = pow(resultColor.e[1], gamma);
				resultColor.e[2] = pow(resultColor.e[2], gamma);

				outputTexture->SetPixel(x, y, resultColor);
			}

			high_resolution_clock::time_point t_end = high_resolution_clock::now();
			totalTime_ns += t_end - t_start;

			int completedLines = y + 1;
			int remainingLines = maxY - completedLines;

			const auto estimateTime = totalTime_ns * remainingLines / completedLines;
			const auto hrs = duration_cast<hours>(estimateTime);
			const auto mins = duration_cast<minutes>(estimateTime - hrs);
			const auto secs = duration_cast<seconds>(estimateTime - hrs - mins);

			std::clog << "\rScanlines remaining: " << remainingLines << "   elapsed time: " << NanoToHHMMSS(totalTime_ns) << "   estimated time: " << NanoToHHMMSS(estimateTime) << "     " << std::flush;
		}

		std::clog << "\rDone in " << NanoToHHMMSS(totalTime_ns) << std::string(64, ' ') << "\n";
	}

private:

	Point3 position;
	Point3 pixelTopLeft;
	Vec3   pixelDeltaU;
	Vec3   pixelDeltaV;
	Vec3   u, v, w;			// Camera frame basis vectors
	Vec3   defocusDiskU;	// Defocus disk horizontal radius
	Vec3   defocusDiskV;	// Defocus disk vertical radius

	shared_ptr<Texture> outputTexture;

	void Initialize() {
		double imageWidth = static_cast<double>(outputTexture->GetResolutionX());
		double imageHeight = static_cast<double>(outputTexture->GetResolutionY());

		position = lookfrom;
		
		double aspect_ratio = imageWidth / imageHeight;

		// Determine viewport dimensions.
		double theta = Deg2Rad(vfov);
		double h = tan(theta / 2);
		double viewportHeight = 2 * h * focusDist;
		double viewportWidth = viewportHeight * aspect_ratio;

		// Calculate the u,v,w unit basis vectors for the camera coordinate frame.
		w = (lookfrom - lookat).normalized();
		u = cross(vup, w).normalized();
		v = cross(w, u);

		// Calculate the vectors across the horizontal and down the vertical viewport edges.
		Vec3 viewportU = viewportWidth * u;		// Vector across viewport horizontal edge
		Vec3 viewportV = viewportHeight * -v;	// Vector down viewport vertical edge

		// Calculate the horizontal and vertical delta vectors from pixel to pixel.
		pixelDeltaU = viewportU / imageWidth;
		pixelDeltaV = viewportV / imageHeight;

		// Calculate the location of the upper left pixel.
		Vec3 viewportTopLeft = position - (focusDist * w) - (viewportU / 2) - (viewportV / 2);
		pixelTopLeft = viewportTopLeft + 0.5 * (pixelDeltaU + pixelDeltaV);

		// Calculate the camera defocus disk basis vectors.
		auto defocusRadius = focusDist * tan(Deg2Rad(defocusAngle / 2));
		defocusDiskU = u * defocusRadius;
		defocusDiskV = v * defocusRadius;
	}

	Ray GetRay(int i, int j) const {
		// Get a randomly sampled camera ray for the pixel at location i,j.
		// Jitters pixelCenter for MSAA
		// Jitters rayOrigin for depth-of-field

		Vec3 pixelCenter = pixelTopLeft + (i * pixelDeltaU) + (j * pixelDeltaV);
		Vec3 pixelSample = pixelCenter + pixelRandomOffset();

		Point3 rayOrigin = (defocusAngle <= 0) ? position : defocusDiskSample();
		Vec3 rayDirection = pixelSample - rayOrigin;

		return Ray(rayOrigin, rayDirection);
	}

	Vec3 pixelRandomOffset() const {
		// Returns a random offset within a pixel square
		double px = RandomRange(-0.5, 0.5);
		double py = RandomRange(-0.5, 0.5);
		return (px * pixelDeltaU) + (py * pixelDeltaV);
	}

	Point3 defocusDiskSample() const {
		// Returns a random point in the camera defocus disk.
		Vec3 p = random_in_unit_disk();
		return position + (p[0] * defocusDiskU) + (p[1] * defocusDiskV);
	}

	Color RayColor(const Ray& r, int depth, const Hittable& world) {
		HitPoint rec;

		// If we've exceeded the ray bounce limit, no more light is gathered.
		if (depth <= 0)
			return Color(0, 0, 0);

		if (world.Hit(r, Interval(0.001, infinity), rec)) {
			Ray outScatteredRay;
			Color attenuation;

			if (rec.mat->scatter(r, rec, attenuation, outScatteredRay)) {
				return attenuation * RayColor(outScatteredRay, depth - 1, world);
			}

			return Color(0, 0, 0);
		}

		Vec3 unit_direction = r.direction.normalized();
		double a = unit_direction.y() * 0.5 + 0.5;
		return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
	}
};
