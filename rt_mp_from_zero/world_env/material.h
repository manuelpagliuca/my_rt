#pragma once

#ifndef MATERIALH
#define MATERIALH

#include "../pcg/pcg_random.hpp"
#include "geometry.h"

// Prototipi
inline const float schlick(const float cosine, const float ref_idx);
bool refract(const vec3& v_in, const vec3& normal, float ni_over_nt, vec3& refracted);
inline const vec3 reflect(const vec3& v_in, const vec3& n);
inline vec3 random_in_unit_sphere();

// Classe material
class material {
public:
	// Input parameters : { Raggio incidente, Record soluzione, Attenuazione, Raggio disperso }
	virtual bool scatter(const ray& r_in, const intersec_record& rec, vec3& attenuation, ray& scattered) const = 0;
};

// Classe lambertian
class lambertian : public material {
public:
	lambertian(const vec3& a) : albedo(a) {}

	virtual bool scatter(const ray& r_in, const intersec_record& rec, vec3& attenuation, ray& scattered) const {
		vec3 target =  rec.p + rec.normal + random_in_unit_sphere(); // Normale della soluzione randomizzato
		scattered = ray(rec.p, target - rec.p);						 // Origine: Vettore soluzione, Direzione: il negativo del vettore soluzione sfasato di qualche valore
		attenuation = albedo;										 // Su attenuazione salvo il colore dell'oggetto (&)
		return true;
	}
	vec3 albedo;
};

// Classe metal
class metal : public material {
public:
	metal(const vec3& a) : albedo(a) {}
	virtual bool scatter(const ray& r_in, const intersec_record& rec, vec3& attenuation, ray& scattered) const {
		vec3 reflected = reflect(unit_vector(r_in.direction), rec.normal); // Vettore riflesso = (Direzione unitaria del raggio incidente, Normale della soluzione)
		scattered = ray(rec.p, reflected);								   // Nel raggio disperso si memorizza un raggio che ha origine dalla soluzione e direzione verso il raggio riflesso
		attenuation = albedo;											   // Nell'attenuazione si salva il colore della classe
		return (dot(scattered.direction, rec.normal) > 0);				   // Restituisce true se il prodotto scalare tra la direzione del raggio dispesso e il vettore normale � > 0
	}
	vec3 albedo;
};

// Classe dielectric
class dielectric : public material {
public:
	dielectric(const float ri) : ref_idx(ri) {}	// Costruttore

	virtual bool scatter(const ray& r_in, const intersec_record& rec, vec3& attenuation, ray& scattered) const {
		
		const vec3 reflected = reflect(r_in.direction, rec.normal); // Vettore riflesso
		vec3 outward_normal;										// Normale esterno
		vec3 refracted;												// Vettore rifratto
		float ni_over_nt;		// IndiceIncidente / IndicePassante
		float reflect_prob;		// Probabilit� di riflessione
		float cosine;			// Coseno

		attenuation = vec3(1.0f, 1.0f, 1.0f); // Set dell' attenuazione, il vetro non assorbe nessun colore

		// RNG (0.0 , 1.0]
		pcg_extras::seed_seq_from<std::random_device> seed_source;
		pcg32 rng(seed_source);
		std::uniform_real_distribution<float> dis(0.0f, 1.0f);
		
		if (dot(r_in.direction, rec.normal) > 0.0f) // -90� < Angle < 90�
		{
			outward_normal = -rec.normal;	// Normale Interno (verso invertito)
			ni_over_nt = ref_idx;			// IdxMateriale (only)
			///cosine = ref_idx * dot(r_in.direction, rec.normal) / r_in.direction.length();
			cosine = dot(r_in.direction, rec.normal) / r_in.direction.length();
			cosine = sqrtf(1.0f - ref_idx * ref_idx * (1.0f - cosine * cosine));
		}
		else // Se l'angolo � ottuso o retto										
		{
			// Il raggio attraversa la sfera
			outward_normal = rec.normal;	// Normale Esterno
			ni_over_nt = 1.0f / ref_idx;	// IdxAria / IdxMateriale
			cosine = -dot(r_in.direction, rec.normal) / r_in.direction.length(); // 
		}
		if (refract(r_in.direction, outward_normal, ni_over_nt, refracted))	//
			reflect_prob = schlick(cosine, ref_idx);
		else
			reflect_prob = 1.0f;	// Probabilit� di riflesso 1.0f se non rifratto
		if (dis(rng) < reflect_prob)
			scattered = ray(rec.p, reflected);
		else
			scattered = ray(rec.p, refracted);
		return true;
	}

	float ref_idx;
};

// Restituisce un normale casuale sulla superficie di una sfera unitaria
inline vec3 random_in_unit_sphere() {
	vec3 p;

	// Seed random point
	pcg_extras::seed_seq_from<std::random_device> seed_source;
	pcg32 rng(seed_source);
	std::uniform_real_distribution<float> dis(-1.0f, 1.0f);

	// Rejection method
	do
	{
		p = vec3(dis(rng), dis(rng), dis(rng));	// Genero il vettore pseudocasuale
		p.make_unit_vector();					// Calcolo il normale
	} while (p.squared_length() >= 1.0f);		// Se la distanza al quadrato � maggiore del raggio al quadrato, reject
	return p;
}

// Riflessione - {Snell's Law}
inline const vec3 reflect(const vec3 & v_in, const vec3 & n) {
	return v_in - 2.0f * dot(v_in, n) * n;	// Poich� v punta verso l'interno della sfera, ad esso si sottrae il doppio prodotto scalare di v rispetto a n.
}

// Rifrazione - {Snell's Law}
bool refract(const vec3 & v_in, const vec3 & normal, float ni_over_nt, vec3 & refracted) {
	const vec3 unit_v_in	 = unit_vector(v_in);		// Rendiamo Il vettore incidente unitario
	const float dt			 = dot(unit_v_in, normal);	// Prodotto scalare, quindi proiezione del vettore incidente sul normale 
	const float discriminant = 1.0f - ni_over_nt * ni_over_nt * (1.0f - dt * dt);

	if (discriminant > 0.)
	{
		refracted = ni_over_nt * (unit_v_in - normal * dt) - normal * sqrtf(discriminant);
		return true;
	}
	else
		return false;
}

// Approssimazione di schlick per il riflesso speculare
inline const float schlick(const float cosine, const float ref_idx) {
	float r0 = (1.0f - ref_idx) / (1.0f + ref_idx);
	r0 = r0 * r0;
	return r0 + (1.0f - r0) * powf((1.0f - cosine), 5.0f);
}

#endif