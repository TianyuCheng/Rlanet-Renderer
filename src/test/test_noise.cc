#include <stdio.h>
#include <math.h>
#include "noiseutils.h"

int main()
{
	int r = 128;
	int h = r * M_PI;
	int height = h;
	int width = r;
	noise::module::Perlin perlin;
	perlin.SetSeed(9527);
	perlin.SetOctaveCount(6);
	perlin.SetFrequency(1.0);

	noise::utils::NoiseMap noiseMap;
	noise::utils::NoiseMapBuilderSphere noiseMapBuilder;

	noiseMapBuilder.SetSourceModule(perlin);
	noiseMapBuilder.SetDestNoiseMap(noiseMap);
	noiseMapBuilder.SetDestSize (width, height);
	noiseMapBuilder.SetBounds (-90.0, 90.0, -90.0, 90.0);
	noiseMapBuilder.Build();

	const float* idata = noiseMap.GetConstSlabPtr();
	for(size_t i = 0; i < width*height; i++) {
		//if (std::abs(idata[i]) > 1e5)
			fprintf(stderr, "\t%f", idata[i]);
		if (i % 16 == 15)
			fprintf(stderr, "\n");
	}

	return 0;
}
