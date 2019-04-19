// Standard Library
#include <random>
#include <algorithm>
#include <cstdlib>
#include <iostream>

// STB_IMAGE
#include "stb\stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb\stb_image_write.h"

// Time Debugging
#include <chrono>
#include <ctime>

// Project files
#include "world_env\sphere.h"
#include "world_env\3d_world.h"
#include "world_env\material.h"
#include "world_env\camera.h"

// Prototipi
inline vec3 pixelColorFunction(const ray& r, Geometry* world, int depth);
inline vec3 lerp(const float t, const vec3& v0, const vec3& v1);
inline vec3 lerp2(const float t, const vec3& v0, const vec3& v1);
inline vec3 random_in_unit_sphere();
inline vec3 reflect(const vec3& v, const vec3& n);
inline float linear_mapped(const vec3& v);

//int const multi = 1;
int const samples = 100;  // Numero di samples da usare per pixel
int const width = 800;	  // Righe di pixels
int const height = 600;   // Colonne di pixels
int const n_objects = 5;  // Numero di oggetti geometrici

// Entry point
int main() {
	auto start = std::chrono::system_clock::now();
	std::vector<uint8_t> image;

	// Lista di oggetti geometrici
	Geometry* list[n_objects];

	// Allocazione degli oggetti nella lista (array) di geometrici
	list[0] = new Sphere(vec3(0.0f, 0.0f, -1.0f), 0.5f, new dielectric(1.5f)); 
	list[1] = new Sphere(vec3(1.5f, 0.0f, -2.0f), 0.5f, new metal(vec3(0.8f, 0.3f, 0.2f)));
	list[2] = new Sphere(vec3(-1.5f, 0.0f, -2.0f), 0.5f, new metal(vec3(0.2f, 0.5f, 0.9f)));
	list[3] = new Sphere(vec3(0.0f, -50.5f, -1.0f), 50.0f, new lambertian(vec3(0.6f, 0.6f, 0.0f)));
	list[4] = new Sphere(vec3(0.0f, 0.0f, 2.0f), 1.4f, new lambertian(vec3(0.0f, 0.0f, 0.0f)));

	// Creo un oggetto che ha il compito di contenere gli altri oggetti 3D
	Geometry* world = new World(list, n_objects);
	
	// Impostazione settings della videocamera
	const vec3 lookfrom(15.0f, 8.0f, 3.0f);
	const vec3 lookat(0.0f, 2.0f, 0.0f);
	const float dist_to_focus = 20.0f;
	const float aperture = 0.1f;
	
	// Dichiarazione e allocazione della videocamera
	camera cam(lookfrom, lookat, vec3(0.0f, 1.0f, 0.0f), 20, float(width) / float(height), aperture, dist_to_focus);
	
	// Seed per generare un float casuale [0,1)
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(0.0f, 1.0f);

	// Devo scandire lo schermo Riga x Colonne
#pragma omp parallel for
	for (int y = height - 1 ; y >= 0; y--)
	{
		for (int x = 0; x < width; x++)
		{	
			// All'interno di questo scope si elabora un singolo pixel
			vec3 col(0.0f, 0.0f, 0.0f);
			for (int s = 0; s < samples; s++)
			{
				const float u = float(x + dis(gen)) / float(width);	 // offset-x
				const float v = float(y + dis(gen)) / float(height); // offset-y
				const ray r = cam.get_ray(u, v);
				///vec3 p = r.point_at_parameter(2.0);
				col += pixelColorFunction(r, world, 0); // Raccolta di tutti i samples all'interno
			}
			col /= float(samples);	// Effettuo una media dei samples

			// 
			uint8_t ir = static_cast<uint8_t>(255.99f * sRGB(col.r()));	
			uint8_t ig = static_cast<uint8_t>(255.99f * sRGB(col.g()));
			uint8_t ib = static_cast<uint8_t>(255.99f * sRGB(col.b()));

			image.push_back(ir);
			image.push_back(ig);
			image.push_back(ib);
		}
	}
	stbi_write_jpg("test.jpg", width, height, 3, image.data(), 100);

	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;
	std::cout << "Elapsed seconds : " << elapsed_seconds.count() << "\n";

	return EXIT_SUCCESS;
}

// Funzione che elabora un pixel
inline vec3 pixelColorFunction(const ray& r, Geometry* world, int depth)
{
	intersec_record record;

	if (world->hit(r, 0.001f, FLT_MAX, record) == true) // Colorazione oggetto
	{
		ray scattered;
		vec3 attenuation;
		if (depth < 50 && record.mat_ptr->scatter(r, record, attenuation, scattered))
			return attenuation * pixelColorFunction(scattered, world, depth + 1);
		else
			return vec3(0.0f, 0.0f, 0.0f);
		//vec3 target = rec.p + rec.normal + random_in_unit_sphere();
		//return 0.5f* color(ray(rec.p, target - rec.p), world);
	}
	else  // Colorazione di cio che non è un oggetto
	{
		vec3 unit_direction = unit_vector(r.direction);	// Versore del raggio
		const float t = 0.5f * (unit_direction.y() + 1.0f);
		return (1.0f - t) * vec3(1.0f, 1.0f, 1.0f) + t * vec3(0.5f, 0.7f, 1.0f);
	}

}

// Interpolazione lineare di due valori (Linear Interpolation)
inline vec3 lerp(const float t, const vec3& v0, const vec3& v1) {
	return (1.0f - t) * v0 + t * v1;
}
// Interpolazione lineare di due valori (old method)
inline vec3 lerp2(const float t, const vec3 & v0, const vec3 & v1) {
	return v0 + t * (v1 - v0);
}

// Mapping Lineare (Linear Mapping)
inline float linear_mapped(const vec3 & v) {
	return 0.5f* (v.y() + 1.0f);
}