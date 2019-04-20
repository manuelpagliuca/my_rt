#pragma once

#ifndef RAYH
#define RAYH
#include "vec3.h"

/*	Il raggio parte dal centro della camera e con l'asse Z interseca l'immagine.
	Modificando tramite offset gli appositi valori su gli assi X e Y si potrà ridirezionare il raggio. */

class ray {
public:
	ray() {}

	// Costruttore
	ray(const vec3 &orig, const vec3 &dir)
	{
		origin = orig;
		direction = dir; 
	}

	// Restituisce vettore che punta al parametro t lungo la linea del raggio
	inline const vec3 point_at_parameter(const float t) const {
		return origin + (direction * t); 
	}

	vec3 origin;	// Vettore che punta all'orgine
	vec3 direction;	// Vettore che indica la direzione
};

#endif // !RAYH
