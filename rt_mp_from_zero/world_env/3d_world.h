#pragma once
#ifndef WORLDH
#define WORLDH
#include "geometry.h"

class World : public Geometry {
public:
	// Costruttori
	World() {
		list = NULL;
		size = 0;
	}

	World(Geometry** __list, int size_n) {
		list = __list;
		size = size_n;
	}

	// Funzioni ereditate
	virtual bool hit(const ray& r, const float t_min, const float t_max, intersec_record& record) const;

	// Membri
	Geometry** list;
	int size;
};

// Funzione che calcola l'intersezione di un raggio con gli oggetti nella lista
bool World::hit(const ray& r, const float t_min, const float t_max, intersec_record& record) const {
	intersec_record temp_rec;	  // Record temporaneo
	bool hit_anything = false;	  // Flag che avvisa se qualcosa è stato colpito	 
	float closest_so_far = t_max; // Un valore che indica la cosa più vicina

	for (int i = 0; i < size; i++)
	{
		// Chiamo la funzione hit astratta del i-esimo oggetto contenuto nella lista
		if (list[i]->hit(r, t_min, closest_so_far, temp_rec)) 
		{
			hit_anything = true;
			closest_so_far = temp_rec.t;
			record = temp_rec;
		}
	}
	return hit_anything;
}

#endif // !WORLDH