#ifndef GEO_H
#define GEO_H

#include "Biomes.h"

struct TerrianInfo {
	int seed;
	static const int NUM_CORNER = 4;
	double height[NUM_CORNER];
	BoimeInfo bio;
};

#endif
