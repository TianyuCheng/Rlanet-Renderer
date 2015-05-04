#ifndef NOISE_H
#define NOISE_H

#include <stdlib.h>

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
	}

	void gen(double* obuf, size_t n)
	{
	}

	static Seed GenSeed() { return random(); }
private:
	Seed seed_;
};

#endif
