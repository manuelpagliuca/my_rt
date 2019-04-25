#pragma once
#ifndef WORLDH
#define WORLDH
#include "geometry.h"

class World : public Geometry {
public:
	// Costruttori
	World() 
	{
		list = NULL;
		size = 0;
	}

	World(Geometry** __list, int size_n) 
	{
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
		if (list[i]->hit(r, t_min, closest_so_far, temp_rec)) /* Se la seguente condizione è verificata, 
																 allora l'i-esimo oggetto ha soluzione rispetto al raggio, 
																 nel record temporaneo vengono salvate tutte le informazioni 
																 riguardanti la soluzione (passaggio per riferimento &)  */
		{
			hit_anything = true;		 // Attivo il flag per segnalare che qualcosa è stato effettivamente colpito
			closest_so_far = temp_rec.t; // Copio la soluzione dell'intersezione
			record = temp_rec;		  	 // Copio il contenuto del record temporaneo, su quello originale
		}
	}
	return hit_anything;
}

#endif // !WORLDH