#pragma once

#ifndef GEOMETRYH
#define GEOMETRYH

#include "ray.h"

// Record contenente le informazioni della collisione
class material;

struct intersec_record {
	float t;
	vec3 p;
	vec3 normal;
	material* mat_ptr;
};

// Superclasse generica per gli oggetti 3D
class Geometry {
public:
	virtual bool hit(const ray& r, const float t_min, const float t_max, intersec_record& record) const = 0;
};

#endif // !GEOMETRYH