#pragma once

#ifndef GEOMETRYH
#define GEOMETRYH

#include "ray.h"

// Record contenente le informazioni della collisione
class material;

struct intersec_record {
	float t;			// Soluzione decimale
	vec3 p;				// Vettore che punta alla soluzione sul raggio
	vec3 normal;		// Normale rispetto alla soluzione (perpendicolare alla superficie dell'oggetto)
	material* mat_ptr;	// Materiale
};

// Superclasse generica per gli oggetti 3D
class Geometry {
public:
	virtual bool hit(const ray& r, const float t_min, const float t_max, intersec_record& record) const = 0;
};

#endif // !GEOMETRYH