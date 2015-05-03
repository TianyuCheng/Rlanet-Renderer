#ifndef MACRO_TILE_H
#define MACRO_TILE_H

#include "TileInfo.h"

constexpr int constlog2(int n)
{
	int ret = 0;
	while (n >>= 1)
		ret++;
	return ret;
}

template<typename TileInfo, int _nchild>
class MacroTile : public Tile<TileInfo> {
public:
	static constexpr int nchild = _nchild;
	static constexpr int childlod = constlog2(nchild);

	typedef MacroTile<TileInfo, nchild> MTile;
	typedef TileSetCursor<TileInfo, int nchild> Cursor;
	typedef std::vector<MTile*> Ancestors;
	typedef MTile* (*ChildrenArrayPointer)[nchild][nchild];
	typedef Seed (*SeedArray)[nchild][nchild];

	MacroTile(const TileInfo& tileinfo, const Seed& seed)
		:Tile(tileinfo, seed) 
	{
	}

	bool is_leaf() const { return pchildren_array_.get() == nullptr; }
	void init_children()
	{
		ChildrenArrayPointer childrenlist = new ChildrenArrayPointer;
		pchildren_array_.reset(childrenlist);
		children_ = std::ref(*childrenlist);

		// Seeds for children
		seeds_.reset(new SeedArray);
		TileInfo::Generator generator(seed_);
		generator.gen(seeds_, nchild*nchild);
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
	bool blit_to(const Coord& axesmins, const Coord& axesmaxs, SecondTile& sectile)
	{
		const auto& secshape = sectile->get_shape();
		typedef SecondTile::Element SecondElement;
		auto blit_cursor = create_blit_cursor(axesmins, axesmaxs, sectile);
		do {
			auto iblock = blit_cursor.current_iblock();
			auto oblock = blit_cursor.current_oblock();
			Splicer splicer(iblock, oblock);
			splicer.splice(oblock.nelem());
		} while (blit_cursor.next());
		return true;
	}

	TileSetCursorPosition find_best_match(const TileInfo& area,
			Ancestors* pancestors = nullptr)
	{
		TileSetCursorPosition pos;
		if (pancestors) {
			pancestors->clear();
			pancestors->emplace_back(this);
		}
		MTile* cursor = this;
		Coord objres = area.get_resolution(0);
		while (cursor->get_resolution(childlod) > objres) {
			if (pancestors)
				pancestors->emplace_back(cursor);
			auto childpos = which(area.init_pos());
			pos.emplace_back(childpos);
			cursor = child(childpos);
		}
		return pos;
	}

	Vec2D<int> which(const Coord& coord)
	{
		Coord childsize = shape/nchild;
		Coord offset = coord - shape_.init_pos();
		return Vec2D<int>(offset.x/childsize.x, offset.y/childsize.y);
	}

	MTile* child(const Vec2D<int>& id)
	{
		if (!pchildren_array_)
			init_children();
		id.clamp_min(Vec2D<int>(0, 0));
		id.clamp_max(Vec2D<int>(nchild-1, nchild-1));
		MTile** ptr = &children_[id.x][id.y];
		if (!*ptr) {
			*ptr = create_chile_at(id);
		}
		return *ptr;
	}

	MTile* create_chile_at(const Vec2D<int>& id) {
		TileInfo ti = shape_;
		ti.d /= nchild;
		ti.res /= nchild;
		ti.coord = id.x * ti.d.x + id.y * ti.d.y;
		return new MTile(ti, TileInfo::Generator::GenSeed()); // TODO GenSeed
	}
protected:
	unique_ptr<ChildrenArrayPointer> pchildren_array_;
	unique_ptr<SeedArray> seeds_;
	std::reference_wrapper<ChildrenArrayPointer> children_;
};

#endif
