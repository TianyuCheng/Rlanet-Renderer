#ifndef NOISECLASSES_H
#define NOISECLASSES_H

#include <vector>
#include <cmath>
#include <stdlib.h>
#include <noise/noise.h>
#include <noiseutils.h>

#include "Geo.h"

// BlankNoise: A noise generator which doesn't do anything
class BlankNoise {
public:
	typedef int Seed;
	BlankNoise(Seed seed)
		:seed_(seed)
	{
	}

	template<typename T>
	void gen(std::vector<T>& obuf)
	{
		for(auto& iter : obuf)
			iter = 256.0; //drand48();
	}

	template<typename T>
	void gen(size_t nline, std::vector<T>& obuf, std::vector<T>& pattern)
	{
		obuf.resize(nline*nline);
		for(auto& iter : obuf)
			iter = 256.0; //*drand48();
	}

	void gen(double* obuf, size_t n)
	{
		for(size_t i = 0; i < n; i++)
			obuf[i] = 256.0; //*drand48();
	}

	static Seed GenSeed() { return random(); }
private:
	Seed seed_;
};

template<typename T>
inline void downsample(T& t, const T& s1, const T& s2, const T& s3, const T& s4)
{
	t = (s1 + s2 + s3 + s4)/4;
}

template<>
inline void downsample(GeoInfo& t, const GeoInfo& s1, const GeoInfo& s2, const GeoInfo& s3, const GeoInfo& s4)
{
	t.height = (s1.height + s2.height + s3.height + s4.height)/4;
	//t.humidity = (s1.humidity + s2.humidity + s3.humidity + s4.humidity)/4;
#if 0
	float peak1 = s1.height+s1.dheight; float peak = peak1;
	float peak2 = s2.height+s2.dheight; peak = std::max(peak, peak2);
	float peak3 = s3.height+s3.dheight; peak = std::max(peak, peak3);
	float peak4 = s4.height+s4.dheight; peak = std::max(peak, peak4);
	float trough1 = s1.height-s1.dheight; float trough = trough1;
	float trough2 = s2.height-s2.dheight; trough = std::min(trough, trough2);
	float trough3 = s3.height-s3.dheight; trough = std::min(trough, trough3);
	float trough4 = s4.height-s4.dheight; trough = std::min(trough, trough4);
	t.dheight = (peak - trough)/2;
#endif
	t.dheight = (s1.dheight + s2.dheight + s3.dheight + s4.dheight)/8;
}


template<typename T>
void downsample(size_t nline, const std::vector<T>& higher, std::vector<T>& lower)
{
	if (!lower.empty())
		return ;
	size_t n = nline / 2;
	lower.resize(n*n);
	for(size_t i = 0; i < lower.size(); i++) {
		downsample(lower[i],
			higher[2*i],
			higher[2*i+1],
			higher[2*i+n],
			higher[2*i+n+1]);
	}
}

inline void geo_expand(GeoInfo& detail, const GeoInfo& pattern, float noise)
{
#if 0
	float ph = pattern.height;
	float pdh = pattern.dheight;
	float nh = ph + noise * pdh;
	detail.height = nh;
	float ndh = std::min(
			std::abs(nh - (ph - pdh)),
			std::abs(nh - (ph+pdh))
			);
	ndh = std::min(
			std::abs(nh - ph),
			ndh);
	detail.dheight = ndh;
#endif
#if 1
	float ph = pattern.height;
	float pdh = pattern.dheight;
	float nh = ph + noise * pdh/2;
	detail.height = nh;
	detail.dheight = pdh/2;
#else
	detail.height = noise;
	detail.dheight = pattern.dheight/2;
#endif
}

inline void geo_expand(float& detail, const float& pattern, float noise)
{
	detail = pattern;
}

inline void geo_set_from_noise(GeoInfo& geo, float noise, int height)
{
	geo.dheight = TOP_TILE_DHEIGHT / 16;
	geo.height = TOP_TILE_DHEIGHT * noise;
}

inline void geo_set_from_noise(float& geo, float noise, int height)
{
	geo = noise;
}

inline void show(const GeoInfo &ob)
{
	fprintf(stderr, "\t%f (%e)", ob.height, ob.dheight);
}

inline void show(const float &ob)
{
	fprintf(stderr, "\t%f", ob);
}

class GeoNoise {
public:
	typedef long int Seed;
	GeoNoise(Seed seed)
		:seed_(seed)
	{
	}

	void gen_subseed(std::vector<Seed>& obuf)
	{
#if 0
		struct drand48_data rd;
		srand48_r((unsigned int)seed_, &rd);
		for(auto& iter : obuf) {
			long int ret;
			lrand48_r(&rd, &ret);
			iter = (Seed)ret;
		}
#else
		srand48((unsigned int)seed_);
		for(auto& iter : obuf)
			iter = lrand48();
#endif
	}

	void gen_lodseeds(std::vector<Seed>& obuf)
	{
#if 0
		struct drand48_data rd;
		srand48_r((unsigned int)~seed_, &rd);
		for(auto& iter : obuf) {
			long int ret;
			lrand48_r(&rd, &ret);
			iter = (Seed)ret;
		}
#else
		srand48((unsigned int)~seed_);
		for(auto& iter : obuf)
			iter = lrand48();
#endif
	}

	template<typename T>
	void gen_terrain(std::vector<T>& obuf, int height, int width)
	{
		noise::module::Perlin perlin;
		perlin.SetSeed(seed_);
		perlin.SetOctaveCount(6);
		perlin.SetFrequency(1.0);

		height += 1;

		noise::utils::NoiseMap noiseMap;
		noise::utils::NoiseMapBuilderSphere noiseMapBuilder;
		//noise::utils::NoiseMapBuilderPlane noiseMapBuilder;

		noiseMapBuilder.SetSourceModule(perlin);
		noiseMapBuilder.SetDestNoiseMap(noiseMap);
		noiseMapBuilder.SetDestSize (width, height);
		noiseMapBuilder.SetBounds (-90.0, 90.0, -90.0, 90.0);
		//noiseMapBuilder.SetBounds (0.0, 1024.0, 0.0, 1024.0);
		noiseMapBuilder.Build();

		const float* idata = noiseMap.GetConstSlabPtr();
		idata += width;
#if 0
		fprintf(stderr, "\tSEED\t%ld, SIZE %lu, width %d, height %d\n", seed_, obuf.size(), height, width);
		size_t s = std::min(64UL, obuf.size());
		for(size_t i = 0; i < s; i++) {
			fprintf(stderr, "\t%f", fabsf(idata[i]));
			if (i % 16 == 15)
				fprintf(stderr, "\n");
		}
		fprintf(stderr, "========================================\n");
#endif
#if 0
		size_t ip = 0;
		for(size_t i = 0; i < obuf.size(); i++) {
			if (fabsf(idata[i]) > 0*1e-6) {
				fprintf(stderr, "\t%f %d", fabsf(idata[i]), fabsf(idata[i]) > 1e6);
				ip++;
			}
			if (ip % 16 == 15)
				fprintf(stderr, "\n");
		}
#endif
		for(size_t i = 0; i < obuf.size(); i++) {
			geo_set_from_noise(obuf[i], idata[i], width);
		}
		showall(obuf);
	}


	template<typename T>
	void showall(std::vector<T>& obuf)
	{
#if 0
		size_t s = std::min(64UL, obuf.size());
		for(size_t i = 0; i < s; i++) {
			show(obuf[i]);
			if (i % 16 == 15)
				fprintf(stderr, "\n");
		}
#endif
	}

	template<typename T>
	void gen_from_pattern(size_t higher_nline,
			size_t pattern_nline,
			std::vector<T>& obuf,
			std::vector<T>& pattern,
			const Seed& patternseed
			)
	{
		if (!obuf.empty())
			return ;
		obuf.resize(higher_nline*higher_nline);
		noise::module::Perlin perlin;
		perlin.SetSeed(patternseed);
		perlin.SetOctaveCount(6);
		perlin.SetFrequency(1.0);

		noise::utils::NoiseMap noiseMap;
		noise::utils::NoiseMapBuilderSphere noiseMapBuilder;
		noiseMapBuilder.SetSourceModule(perlin);
		noiseMapBuilder.SetDestNoiseMap(noiseMap);
		noiseMapBuilder.SetDestSize(int(higher_nline)+1, int(higher_nline));
		noiseMapBuilder.SetBounds (-90.0, 90.0, -90.0, 90.0);
		noiseMapBuilder.Build();

		const float* idata = noiseMap.GetConstSlabPtr();
		idata += higher_nline;
		for(size_t x = 0; x < higher_nline; x++) {
			for(size_t y = 0; y < higher_nline; y++) {
				size_t id = x * higher_nline + y;
				size_t px = x / 2;
				size_t py = y / 2;
				size_t pid = px * pattern_nline + py;
				geo_expand(obuf[id], pattern[pid], idata[id]);
			}
		}
		showall(obuf);
	}

#if 0
	void gen(double* obuf, size_t n)
	{
		for(size_t i = 0; i < n; i++)
			obuf[i] = 256.0; //*drand48();
	}
#endif

	static Seed GenSeed() { return random(); }
private:
	Seed seed_;
	float res_;
};
#endif
