#ifndef MACRO_TILE_H
#define MACRO_TILE_H

#include "Tile.h"
#include "TileBlitter.h"
#include <boost/integer/static_log2.hpp> 

#if 0
constexpr int constlog2(int n)
{
	int ret = 0;
	while (n >>= 1)
		ret++;
	return ret;
}
#endif

template<typename TileInfo, int _nchild>
class MacroTile : public Tile<TileInfo> {
public:
	static constexpr int nchild = _nchild;
	static constexpr int childlod = boost::static_log2<nchild>::value;

	typedef MacroTile<TileInfo, nchild> MTile;
	typedef MTile *MTilePointer;
	typedef TileHierarchyCursor<MTile> Cursor;
	typedef std::vector<MTile*> Ancestors;
	typedef MTilePointer ChildrenPointerArray[nchild][nchild];
	typedef typename TileInfo::TileSeed Seed;
	typedef Seed (*SeedArray)[nchild][nchild];
	typedef typename TileInfo::TileElement Element;
	typedef typename TileInfo::Coordinate Coord;

	MacroTile(const TileInfo& tileinfo, const Seed& seed)
		:Tile<TileInfo>(tileinfo, seed), pchildren_array_(nullptr)
	{
	}

	~MacroTile()
	{
		free(pchildren_array_);
	}


	bool is_leaf() const { return pchildren_array_ == nullptr; }
	void init_children()
	{
		// Sometimes C++ makes things more complicated
		// Here you CAN'T allocate the expensive (2K bytes for nchild=16) ChildrenPointerArray direclty with new
		pchildren_array_ = (ChildrenPointerArray*)malloc(sizeof(ChildrenPointerArray));
		// Seeds for children
		seeds_.resize(nchild*nchild);
		typename TileInfo::Generator generator(this->seed_);
		generator.gen(seeds_);
	}
	/*
	 * Transfer a part of the whole terrain hierarchy to a second tile
	 * object
	 *
	 * One typical use case is to sample a part of terrain into a height
	 * map.
	 *
	 * Note: need a working MacroTileBlitCursor class to work, which can
	 * walk among tiles.
	 */
	template<typename SecondTile, typename Splicer = TileSplicer<Element> >
	bool blit_to(SecondTile& sectile)
	{
		auto blit_cursor = create_blitter(sectile, *this);
		do {
			auto iblock = blit_cursor.current_iblock();
			auto oblock = blit_cursor.current_oblock();
			Splicer splicer(iblock, oblock);
			splicer.splice(oblock.nelem());
		} while (blit_cursor.next());
		return true;
	}

	Cursor find_best_match(const TileInfo& area,
			Ancestors* pancestors = nullptr)
	{
		Ancestors ancestors;
		TileSetCursorPosition pos;
		ancestors.emplace_back(this);
		MTile* cursor = this;
		double objres = area.get_resolution(0);
		while (cursor->get_resolution(childlod) > objres) {
			ancestors.emplace_back(cursor);
			auto childpos = which(area.init_pos());
			pos.emplace_back(childpos);
			cursor = child(childpos);
		}
		// TODO: LOD
		if (pancestors) {
			*pancestors = ancestors;
		}
		return Cursor(pos, ancestors);
	}

	Vec2D<int> which(const Coord& coord)
	{
		Coord childsize = this->shape_.shape/nchild;
		Coord offset = coord - this->shape_.init_pos();
		return Vec2D<int>(offset.x/childsize.x, offset.y/childsize.y);
	}

	MTile* child(Vec2D<int> id) // We need clamp, so NO const&
	{
		if (!pchildren_array_)
			init_children();
		id.clamp_min(Vec2D<int>(0, 0));
		id.clamp_max(Vec2D<int>(nchild-1, nchild-1));
		MTile** ptr = &((*pchildren_array_)[id.x][id.y]);
		if (!*ptr) {
			*ptr = create_chile_at(id);
		}
		return *ptr;
	}

	MTile* create_chile_at(const Vec2D<int>& id) {
		TileInfo ti = this->shape_;
		ti.shape /= nchild;
		ti.res /= nchild;
		ti.init_coord.x = id.x * ti.shape.x;
		ti.init_coord.y = id.y * ti.shape.y;
		return new MTile(ti, TileInfo::Generator::GenSeed()); // TODO GenSeed
	}
protected:
	ChildrenPointerArray* pchildren_array_;
	std::vector<Seed> seeds_;
};

typedef MacroTile<TerrainTileInfo, 16> Zearth; // The Earth

#endif
