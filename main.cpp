#define STBIW_ASSERT(x)
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "RTWeekend.h"

#include "HittableList.h"
#include "Material.h"
#include "Sphere.h"
#include "Camera.h"
#include "Texture.h"

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

	auto materialGround = make_shared<Lambertian>(Color(0.8, 0.8, 0.0));
	auto materialCenter = make_shared<Lambertian>(Color(0.7, 0.3, 0.3));
	auto materialLeft = make_shared<Metal>(Color(0.8, 0.8, 0.8), 0.3);
	auto materialRight = make_shared<Metal>(Color(0.8, 0.6, 0.2), 1.0);

	world.add(make_shared<Sphere>(Point3(0.0, -100.5, -1.0), 100.0, materialGround));
	world.add(make_shared<Sphere>(Point3(0.0, 0.0, -1.0), 0.5, materialCenter));
	world.add(make_shared<Sphere>(Point3(-1.0, 0.0, -1.0), 0.5, materialLeft));
	world.add(make_shared<Sphere>(Point3(1.0, 0.0, -1.0), 0.5, materialRight));

	Camera camera(outputTexture);
	camera.samplesPerPixel = 20;
	camera.maxRayBounces = 50;
	camera.Render(world);
	
	if (!outputTexture->SaveToFile("output.png"))
		return 1;

	return 0;
}