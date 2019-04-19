#pragma once

#ifndef MATERIALH
#define MATERIALH

#include "geometry.h"

// Prototipi
inline const float schlick(const float cosine, const float ref_idx);
bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted);
inline vec3 reflect(const vec3& v, const vec3& n);
inline vec3 random_in_unit_sphere();

// Classe material
class material {
public:
	virtual bool scatter(const ray& r_in, const intersec_record& rec, vec3& attenuation, ray& scattered) const = 0;
};

// Classe lambertian
class lambertian : public material {
public:
	lambertian(const vec3& a) : albedo(a) {}
	virtual bool scatter(const ray& r_in, const intersec_record& rec, vec3& attenuation, ray& scattered) const {
		vec3 target = rec.p + rec.normal + random_in_unit_sphere();
		scattered = ray(rec.p, target - rec.p);
		attenuation = albedo;
		return true;
	}
	vec3 albedo;
};

// Classe metal
class metal : public material {
public:
	metal(const vec3& a) : albedo(a) {}
	virtual bool scatter(const ray& r_in, const intersec_record& rec, vec3& attenuation, ray& scattered) const {
		vec3 reflected = reflect(unit_vector(r_in.direction), rec.normal);
		scattered = ray(rec.p, reflected);
		attenuation = albedo;
		return (dot(scattered.direction, rec.normal) > 0);
	}
	vec3 albedo;
};

// Classe dielectric
class dielectric : public material {
public:
	dielectric(float ri) : ref_idx(ri) {}
	virtual bool scatter(const ray& r_in, const intersec_record& rec, vec3& attenuation, ray& scattered) const {
		vec3 outward_normal;
		vec3 reflected = reflect(r_in.direction, rec.normal);
		float ni_over_nt;
		attenuation = vec3(1.0f, 1.0f, 1.0f);
		vec3 refracted;
		float reflect_prob;
		float cosine;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> dis(0.0f, 1.0f);

		if (dot(r_in.direction, rec.normal) > 0.0f) {
			outward_normal = -rec.normal;
			ni_over_nt = ref_idx;
			//cosine = ref_idx * dot(r_in.direction, rec.normal) / r_in.direction.length();
			cosine = dot(r_in.direction, rec.normal) / r_in.direction.length();
			cosine = sqrt(1 - ref_idx * ref_idx * (1 - cosine * cosine));
		}
		else {
			outward_normal = rec.normal;
			ni_over_nt = 1.0f / ref_idx;
			cosine = -dot(r_in.direction, rec.normal) / r_in.direction.length();
		}
		if (refract(r_in.direction, outward_normal, ni_over_nt, refracted))
			reflect_prob = schlick(cosine, ref_idx);
		else
			reflect_prob = 1.0f;
		if (dis(gen) < reflect_prob)
			scattered = ray(rec.p, reflected);
		else
			scattered = ray(rec.p, refracted);
		return true;
	}

	float ref_idx;
};

/* Prendo un cubo unitario all'interno del raggio della sfera e ne restituisco il normale */
inline vec3 random_in_unit_sphere() {
	vec3 p;
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(0.0f, 1.0f);

	do
	{
		p = 2.0f * vec3(dis(gen), dis(gen), dis(gen));
		p -= vec3(1.0f, 1.0f, 1.0f);
		p.make_unit_vector();	// Calcolo il normale
	} while (p.squared_length() >= 1.0f);
	return p;
}

// Riflessione
inline vec3 reflect(const vec3 & v, const vec3 & n) {
	return v - 2.0f * dot(v, n) * n;
}

// Rifrazione
bool refract(const vec3 & v, const vec3 & n, float ni_over_nt, vec3 & refracted) {
	vec3 uv = unit_vector(v);
	const float dt = dot(uv, n);
	const float discriminant = 1.0f - ni_over_nt * ni_over_nt * (1.0f - dt * dt);

	if (discriminant > 0.) {
		refracted = ni_over_nt * (uv - n * dt) - n * sqrt(discriminant);
		return true;
	}
	else
		return false;
}

// Approssimazione di schlick per il riflesso speculare
inline const float schlick(const float cosine, const float ref_idx) {
	float r0 = (1.0f - ref_idx) / (1.0f + ref_idx);
	r0 = r0 * r0;
	return r0 + (1.0f - r0) * pow((1.0f - cosine), 5.0f);
}

#endif