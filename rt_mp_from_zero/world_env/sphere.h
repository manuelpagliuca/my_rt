#pragma once

#ifndef SPHEREH
#define SPHEREH
#include "geometry.h"

class Sphere : public Geometry {
public:
	// Costruttori
	Sphere() {
		center = NULL;
		radius = 0.0f;
		mat_ptr = NULL;
	}

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
	const vec3 oc = r.origin - this->center;		// Vettore che va dall'origine verso il centro della sfera
	const float a = dot(r.direction, r.direction);	// Quadrato dell'equazione
	const float b = dot(oc, r.direction);			// Termine di primo grado
	const float c = dot(oc, oc) - radius * radius;	// Termine noto
	const float discriminant = (b * b) - (a * c);	// delta -> (b^2-4ac o b^2-ac)

	if (discriminant > 0.0f)
	{
		float temp = (-b - sqrt(discriminant)) / a;

		if (temp > t_min && temp < t_max)
		{
			record.t = temp;											// Memorizzo la soluzione sul record
			record.p = r.point_at_parameter(record.t);					// Memorizzo il vettore della soluzione sul record
			record.normal = (record.p - this->center) / this->radius;	// Memorizzo il vettore normale rispetto al vettore soluzione sul record
			record.mat_ptr = this->mat_ptr;								// Memorizzo il materiale sul record
			return true;
		}

		temp = (-b + sqrt(discriminant)) / a;

		if (temp > t_min && temp < t_max)
		{
			record.t = temp;											// Memorizzo la soluzione sul record
			record.p = r.point_at_parameter(record.t);					// Memorizzo il vettore della soluzione sul record
			record.normal = (record.p - this->center) / this->radius;	// Memorizzo il vettore normale rispetto al vettore soluzione sul record
			record.mat_ptr = this->mat_ptr;								// Memorizzo il materiale sul record
			return true;
		}

	}
	return false;
}

#endif // !SPHEREH
