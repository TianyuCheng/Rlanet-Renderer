#ifndef GEO_H
#define GEO_H

#include <boost/range/irange.hpp>
#include "Biomes.h"

struct GeoInfo {
	float height; // Average height
	//float humidity;
	float dheight; // variance
};

struct GeoHeightExtractor {
	void ncopy(GeoInfo* src, size_t n, float* dst)
	{
		for(size_t i : boost::irange(0UL, n))
			dst[i] = src[i].height;
	}
};

#if 0
struct GeoHumidityExtractor {
	void ncopy(GeoInfo* src, size_t n, float* dst)
	{
		for(size_t i : boost::irange(0UL, n))
			dst[i] = src[i].humidity;
	}
};
#endif

#define TOP_TILE_DHEIGHT	(1.0)

#endif
