#define STBIW_ASSERT(x)
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "RTWeekend.h"

#include "HittableList.h"
#include "Sphere.h"
#include "Texture.h"
#include "Camera.h"

#include <iostream>

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

int main()
{
	STBI_DISABLE_PNG_COMPRESSION

	// initialise output image
	int imageWidth = 1280;
	int imageHeight = 720;
	auto outputTexture = shared_ptr<Texture>(new Texture(imageWidth, imageHeight));

	HittableList world;

	world.add(make_shared<Sphere>(Point3(0, 0, -1), 0.5));
	world.add(make_shared<Sphere>(Point3(0, -100.5, -1), 100));

	Camera camera(outputTexture);
	camera.samplesPerPixel = 20;
	camera.maxRayBounces = 50;
	camera.Render(world);
	
	outputTexture->SaveToFile("result.png");

	return 0;
}