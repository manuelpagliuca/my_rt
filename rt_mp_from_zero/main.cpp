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

// Entry point
int main(void) {
	//int const multi = 1;
	int const width = 800;			// Righe di pixels
	int const height = 600;		// Colonne di pixels
	int const n_objects = 8;		// Numero di oggetti geometrici
	int const samples = 10;			// Numero di samples da usare per pixel

	int y;

	auto start = std::chrono::system_clock::now();
	std::vector<uint8_t> image;

	// Lista di oggetti geometrici
	Geometry* list[n_objects];

	// Allocazione degli oggetti nella lista (array) di geometrici
	list[0] = new Sphere(vec3(0.0f, 0.0f, -1.0f), 0.5f, new dielectric(1.5f)); 
	list[1] = new Sphere(vec3(1.5f, 0.0f, -2.0f), 0.5f, new metal(vec3(0.8f, 0.3f, 0.2f)));
	list[2] = new Sphere(vec3(-1.5f, 0.0f, -2.0f), 0.5f, new metal(vec3(0.2f, 0.5f, 0.9f)));
	list[3] = new Sphere(vec3(0.0f, -50.5f, -1.0f), 50.0f, new lambertian(vec3(0.1f, 0.4f, 0.1f)));
	list[4] = new Sphere(vec3(-2.0f, 0.8f, 0.0f), 1.4f, new lambertian(vec3(0.3f, 0.3f, 0.8f)));
	list[5] = new Sphere(vec3(-0.8f, 1.8f, 4.0f), 2.6f, new dielectric(1.5f));  /// 2.417f
	list[6] = new Sphere(vec3(3.0f, 0.9f, -3.0f), 1.5f, new metal(vec3(0.4f, 0.7f, 0.1f)));
	list[7] = new Sphere(vec3(-3.0f, 2.5f, -6.0f), 3.6f, new metal(vec3(0.1f, 0.1f, 0.1f)));

	// Creo un oggetto che ha il compito di contenere gli altri oggetti 3D
	Geometry* world = new World(list, n_objects);
	
	// Impostazione settings della videocamera
	const vec3 lookfrom(15.0f, 3.0f, 15.0f);
	const vec3 lookat(0.0f, 2.0f, 0.0f);
	const vec3 vup(0.0f, 1.0f, 0.0f);
	const float dist_to_focus = 20.0f;
	const float aperture = 0.1f;
	const float aspect = 20.0f;

	// Dichiarazione e allocazione della videocamera
	camera cam(lookfrom, lookat, vup, aspect, float(width) / float(height), aperture, dist_to_focus);
	
	// Seed per generare un float casuale [0,1)
	pcg_extras::seed_seq_from<std::random_device> seed_source;
	pcg32 rng(seed_source);
	std::uniform_real_distribution<float> dis(0.0f, 1.0f);

	/*
		0,h------------------------------w,h   Punto di altezza&larghezza max
		 -								  -
		 -								  -
		 -								  -
		 -								  -
		 -								  -
		 -								  -
		 -								  -
		 -								  -
		 -								  -
		 0,0------------------------------w,0  
		Sono necessari due cicli height x width
	*/

#pragma omp parallel for
	for (y = height; y > 0; y--)		// Colonna
	{
		fprintf(stderr, "\rRendering (%d samples) %5.2f%%", samples, 100.0f*float(height - y)/float(height));

		for (int x = 0; x < width; x++) // Riga
		{	
			vec3 col(0.0f, 0.0f, 0.0f);	// Pixel
			for (int s = 0; s < samples; s++)	// Campionamento sul singolo pixel
			{
				const float u = float(x + dis(rng)) / float(width);	 // Ordinata pseudo casuale
				const float v = float(y + dis(rng)) / float(height); // Ascissa pseudo casuale
				const ray r = cam.get_ray(u, v);	// La camera restituisce un raggio che attraversa il sample (o sub-pixel)
				///vec3 p = r.point_at_parameter(2.0);
				col += pixelColorFunction(r, world, 0); // Restituisco il colore di ciò che interseca il raggio nel sub-pixel casuale, il valore viene iterativamente accumulato all'interno del pixel.
			}
			col /= float(samples);	// Divido i colori accumulate nel pixel per il numero di samples utilizzati, effettuo quindi una media dei colori all'interno del pixel.

			uint8_t ir = static_cast<uint8_t>(255.99f * sRGB(col.r()));	
			uint8_t ig = static_cast<uint8_t>(255.99f * sRGB(col.g()));
			uint8_t ib = static_cast<uint8_t>(255.99f * sRGB(col.b()));
			// Memorizza la tripla del pixel nel vettore
			image.push_back(ir);
			image.push_back(ig);
			image.push_back(ib); 
		}
	}
	fprintf(stderr, "\rRendering (%d samples) %5.2f%%\n", samples, 100.0f*float(height - y) / float(height));
	// Output image
	stbi_write_jpg("output.jpg", width, height, 3, image.data(), 100);

	auto end = std::chrono::system_clock::now();

	std::chrono::duration<double> elapsed_seconds = end - start;
	std::cout << "It tooks " << elapsed_seconds.count() << " seconds \n\n\n";

	return EXIT_SUCCESS;
}

// Funzione che elabora un pixel
inline vec3 pixelColorFunction(const ray& r, Geometry* world, int depth)
{
	intersec_record record;

	if (world->hit(r, 0.001f, FLT_MAX, record)) /*	Se nel mondo avviene una intersezione tra raggio e oggetti,
													in tal caso la condizione viene verificata e le informazioni
													riguardanti la soluzione sono salvate sul record (passato per riferimento &) */
	{
		ray scattered;		// Raggio disperso
		vec3 attenuation;	// Attenuazione

		if (depth < 50 && record.mat_ptr->scatter(r, record, attenuation, scattered))
			return attenuation * pixelColorFunction(scattered, world, depth + 1);
		else
			return vec3(0.0f, 0.0f, 0.0f);
		//vec3 target = rec.p + rec.normal + random_in_unit_sphere();
		//return 0.5f* color(ray(rec.p, target - rec.p), world);
	}
	else // Colorazione di cio che non è un oggetto
	{
		vec3 unit_direction = unit_vector(r.direction);	// Versore del raggio
		const float t = 0.5f * (unit_direction.y() + 1.0f);
		return (1.0f - t) * vec3(0.0f, 0.0f, 0.0f) + t * vec3(1.0f, 1.0f, 1.0f);
	}

}