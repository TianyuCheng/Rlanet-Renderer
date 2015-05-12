#ifndef GEO_H
#define GEO_H

#include <boost/range/irange.hpp>
#include "Biomes.h"

struct GeoInfo {
	float height; // Average height
	float humidity;
	float dheight; // variance
};

struct GeoHeightExtractor {
	void ncopy(GeoInfo* src, size_t n, float* dst)
	{
		for(size_t i : boost::irange(0UL, n))
			dst[i] = src[i].height;
	}
};

struct GeoHumidityExtractor {
	void ncopy(GeoInfo* src, size_t n, float* dst)
	{
		for(size_t i : boost::irange(0UL, n))
			dst[i] = src[i].humidity;
	}
};

#define TOP_TILE_DHEIGHT	(1.2)

#endif
