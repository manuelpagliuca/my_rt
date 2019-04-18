#pragma once

#ifndef SPHEREH
#define SPHEREH
#include "geometry.h"

class Sphere : public Geometry {
public:
	// Costruttori
	Sphere() {}
	Sphere(const vec3& cen, float rad, material* m) 
	{	center = cen;
		radius = rad;
		mat_ptr = m; 
	}
	
	// Metodo per l'intersezione con il raggio
	virtual bool hit(const ray& r, const float t_min, const float t_max, intersec_record& record) const ;
	
	// Membri
	vec3 center;
	float radius;
	material* mat_ptr;
};

// Funzione che calcola L'intersezione raggio-sfera
bool Sphere::hit(const ray& r, const float t_min, const float t_max, intersec_record& record) const {
	const vec3 oc = r.origin - center;

	const float a = dot(r.direction, r.direction);
	const float b = dot(oc, r.direction);
	const float c = dot(oc, oc) - radius * radius;
	const float discriminant = (b * b) - (a * c);

	if (discriminant > 0.0f)
	{
		float temp = (-b - sqrt(discriminant)) / a;
		if (temp < t_max && temp > t_min)
		{
			record.t = temp;
			record.p = r.point_at_parameter(record.t);
			record.normal = (record.p - center) / radius;
			record.mat_ptr = mat_ptr;
			return true;
		}

		temp = (-b + sqrt(discriminant)) / a;

		if (temp < t_max && temp > t_min)
		{
			record.t = temp;
			record.p = r.point_at_parameter(record.t);
			record.normal = (record.p - center) / radius;
			record.mat_ptr = mat_ptr;
			return true;
		}

	}
	return false;

}

#endif // !SPHEREH
