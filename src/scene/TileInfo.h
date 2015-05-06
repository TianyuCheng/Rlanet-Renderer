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

	TileShape(const TileShape& t)
		:init_coord(t.init_coord),shape(t.shape),res(t.res)
	{
	}
};

// We also need TileInfo concenpt for sampling purpose
struct TerrainTileInfo : public TileShape<double> {
#if 0
	double theta_, dtheta_;
	double phi_, dphi_;
#endif
	TerrainTileInfo(TileShape<double>& tileshape)
		:TileShape(tileshape)
	{
		ishape_ = shape/res;
		nelem_ = ishape_.x * ishape_.y;
	}
	typedef double TileElement; // Height
	typedef Vec2D<double> Coordinate; // Coordinate
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

	size_t nelement() const { return nelem_; }
	size_t nline() const { return ishape_.x; }
	Coordinate init_pos() const { return init_coord; }
	Coordinate tail_pos() const { return init_coord + shape; }
	double get_resolution(int LODLevel) const {
		return res/(1<<LODLevel);
	}

private:
	Vec2D<int> ishape_;
	size_t nelem_;
};

#endif
