#ifndef BOIMES_H
#define BOIMES_H

enum BOIMES {
	FOREST,
	SNOW_FOREST,
	PRAIRIE,
	SWAMP,
	DESERT,
	MOUNTAIN,
	TUNDRA,
	OCEAN,
};

struct BoimeInfo {
	BOIMES biotype;
};

#endif
