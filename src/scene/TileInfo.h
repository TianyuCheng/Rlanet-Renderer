#ifndef TILE_INFO_H
#define TILE_INFO_H

#include "Vec2D.h"
#include "Noise.h"

template<typename T>
struct TileShape {
	Vec2D<T> init_coord;
	Vec2D<T> shape;
	T res; // Resolution

	TileShape()
	{
	}

	TileShape(const Vec2D<T>& ic, const Vec2D<T>& is, const T& ir)
		:init_coord(ic), shape(is), res(ir)
	{
	}

	TileShape(const TileShape& t)
		:init_coord(t.init_coord),shape(t.shape),res(t.res)
	{
	}
};

// We also need TileInfo concenpt for sampling purpose
struct TerrainTileInfo : public TileShape<float> {
	TerrainTileInfo(const TileShape<float>& tileshape)
		:TileShape(tileshape)
	{
		recalibre(res);
	}
	typedef float FloatType;
	typedef float TileElement; // Height
	typedef Vec2D<FloatType> Coordinate; // Coordinate
	typedef BlankNoise Generator;
	typedef typename Generator::Seed TileSeed; // Seed for noise generation

	ssize_t get_linear(const Coordinate& coord) const
	{
		auto icoord = get_block(coord);
		return icoord.x * ishape_.y + icoord.y;
	}

	Vec2D<int> get_block(const Coordinate& coord) const
	{
		Vec2D<int> ret;
		return (coord - init_coord) / res;
	}

	Coordinate get_coord(const Vec2D<int>& id, int LODLevel) const
	{
		Coordinate ret(init_coord);
		FloatType lodres = get_resolution(LODLevel);
		ret.x += lodres * id.x;
		ret.y += lodres * id.y;
		return ret;
	}

	size_t nelement() const { return nelem_; }
	size_t nline() const { return ishape_.x; }
	size_t nelem_in_line() const { return ishape_.y; }
	Coordinate init_pos() const { return init_coord; }
	Coordinate tail_pos() const { return init_coord + shape; }
	FloatType get_resolution(int LODLevel) const {
		return res*(1<<LODLevel);
	}

	void recalibre(double newres)
	{
		res = newres;
		ishape_ = shape/res;
		nelem_ = ishape_.x * ishape_.y;
	}

private:
	Vec2D<int> ishape_;
	size_t nelem_;
};

#endif
