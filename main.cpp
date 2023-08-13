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

	auto ground_material = make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
	world.add(make_shared<Sphere>(Point3(0, -1000, 0), 1000, ground_material));

	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			auto choose_mat = Random01();
			Point3 center(a + 0.9 * Random01(), 0.2, b + 0.9 * Random01());

			if ((center - Point3(4, 0.2, 0)).length() > 0.9) {
				shared_ptr<Material> sphere_material;

				if (choose_mat < 0.8) {
					// diffuse
					auto albedo = Color::random() * Color::random();
					sphere_material = make_shared<Lambertian>(albedo);
					world.add(make_shared<Sphere>(center, 0.2, sphere_material));
				}
				else if (choose_mat < 0.95) {
					// metal
					auto albedo = Color::random(0.5, 1);
					auto fuzz = RandomRange(0, 0.5);
					sphere_material = make_shared<Metal>(albedo, fuzz);
					world.add(make_shared<Sphere>(center, 0.2, sphere_material));
				}
				else {
					// glass
					sphere_material = make_shared<Dielectric>(1.5);
					world.add(make_shared<Sphere>(center, 0.2, sphere_material));
				}
			}
		}
	}

	auto material1 = make_shared<Dielectric>(1.5);
	world.add(make_shared<Sphere>(Point3(0, 1, 0), 1.0, material1));

	auto material2 = make_shared<Lambertian>(Color(0.4, 0.2, 0.1));
	world.add(make_shared<Sphere>(Point3(-4, 1, 0), 1.0, material2));

	auto material3 = make_shared<Metal>(Color(0.7, 0.6, 0.5), 0.0);
	world.add(make_shared<Sphere>(Point3(4, 1, 0), 1.0, material3));

	Camera camera(outputTexture);

	// camera transform
	camera.lookfrom = Point3(13, 2, 3);
	camera.lookat = Point3(0, 0, 0);
	camera.vup = Vec3(0, 1, 0);

	// lens settings
	camera.vfov = 20;
	camera.defocusAngle = 0.6;
	camera.focusDist = 10.0;
	
	// render settings
	camera.samplesPerPixel = 500;
	camera.maxRayBounces = 50;

	camera.Render(world);
	
	if (!outputTexture->SaveToFile("output.png"))
		return 1;

	return 0;
}