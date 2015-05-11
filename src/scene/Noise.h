#ifndef NOISECLASSES_H
#define NOISECLASSES_H

#include <vector>
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

#endif
