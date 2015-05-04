#ifndef TILE_BLITTER_H
#define TILE_BLITTER_H

#include "MacroTile.h"

struct TileSetCursorPosition : public std::vector<Vec2D<int>> {
};

inline bool operator<(const TileSetCursorPosition& lhs, const TileSetCursorPosition& rhs)
{
	if (lhs.size() != rhs.size())
		return false;
	auto iter1 = lhs.begin();
	auto iter2 = rhs.begin();
	while (iter1 != lhs.end() && *iter1 != *iter2) {
		iter1++;
		iter2++;
	}
	return *iter1 < *iter2;
}

template<typename TileInfo, int nchild>
class MacroTile;

template<typename MTile>
struct TileHierarchyCursor {
	static constexpr int nchild = MTile::nchild;
	typedef typename MTile::Coord Coord;

	TileHierarchyCursor(const TileSetCursorPosition& pos,
			const std::vector<MTile*>& anc)
		:now(pos), ancestors(anc)
	{
	}

	/*
 	 * Invariant Condition
	 * ancestor[i+1] == ancestors[i].child(now[i]);
	 */
	TileSetCursorPosition now;
	std::vector<MTile*> ancestors;
	int LODLevel;

	MTile* current_tile()
	{
		return ancestors.back();
	}
	/*
 	 * Be careful.
 	 * TileHierarchyCursor does not have a bound other than 0-max
	 *
	 * Blitter cursors should use successor, CR and LF instead.
	 */
	bool up() { return tile_move(-1,0); }
	bool down() { return tile_move(1,0); }
	bool left() { return tile_move(0,-1); }
	bool right() { return tile_move(0,1); }

	double get_resolution()
	{
		return current_tile()->get_resolution(LODLevel);
	}

	bool tile_move(int dx, int dy)
	{
		Vec2D<int> dvec(dx, dy);
		ssize_t iter = now.size() - 1;

		do {
			bool ret = false;
			now[iter] += dvec; // Move cursor at current tile level
			if (!now[iter].wrap(nchild, nchild))
				ret = true; // No need to move higher level cursor
			iter--;
			if (iter > 0) {
				// Maintain ancestors
				ancestors[iter + 1] = ancestors[iter]->child(now[iter+1]);
			}
			if (ret) // Return if done
				return ret;
			// Otherwise proceed to higher level
		} while (iter > 0);
		// OOB apparently
		return false;
	}
};

/*
 * Cursor for blitting data from Tile Hierarchy to height map tile.
 */
template<typename SourceTile, typename TargetTile>
class BlitterFromMTile : public TileHierarchyCursor<SourceTile>
{
public:
	static const int nchild = SourceTile::nchild;
	typedef TileHierarchyCursor<SourceTile> TileCursor;
	typedef typename SourceTile::Element SourceElement;
	typedef typename TargetTile::Element TargetElement;
	typedef typename SourceTile::Coord Coord;

	BlitterFromMTile(const TileSetCursorPosition& start,
			const std::vector<SourceTile*>& anc,
			const TileSetCursorPosition& _mins,
			const TileSetCursorPosition& _maxs,
			TargetTile &_sect
			)
		:TileHierarchyCursor<SourceTile>(start, anc),
		mins(_mins), maxs(_maxs),
		write_pos_(0,0), sectile(_sect)
	{
	}

	TileSetCursorPosition mins, maxs; // Bounds, used by CR LF and successor
	TargetTile &sectile;

	bool oob() const
	{
		if (maxs < this->now || this->now < mins)
			return true;
		return false;
	}
	/*
 	 * Time to learn the original meaning of these two words
	 * CR: move the carriage to the start of CURRENT line
	 * LF: move the carriage to the next line
	 *
	 * Thus, here we require CR+LF to get to the start of new line.
 	 */
	void carriage_return()
	{
		for(size_t i = 0; i < mins.size(); i++) {
			this->now[i].y = mins[i].y;
		}
		write_pos_.y = 0;
	}

	bool line_feed()
	{
		bool ret = this->down();
		write_pos_.x++;
		return oob() && ret;
	}

	void get_blitting_region(Coord& min, Coord& max)
	{
		SourceTile *now = this->current_tile();
		min = now->init_pos();
		max = now->tail_pos();
		min.clamp_min(sectile.init_pos());
		max.clamp_max(sectile.tail_pos());
	}

	TileIO<SourceElement> current_iblock()
	{
		SourceTile *now = this->current_tile();
		Coord min, max;
		get_blitting_region(min, max);
		return now->get_ioblock(min, max, 0);
	}

	TileIO<TargetElement> current_oblock()
	{
		Coord min, max;
		get_blitting_region(min, max);
		return sectile.get_ioblock(min, max, 0);
	}

	bool next()
	{
		this->right(); // Move one step right
		if (!oob())
			return true;
		carriage_return();
		return line_feed();
	}
private:
	Vec2D<int> write_pos_;
};

template<typename SourceTile, typename TargetTile>
BlitterFromMTile<SourceTile, TargetTile>
create_blitter(TargetTile& target, SourceTile& source)
{
	const auto& tileregion = target.get_shape_info();
	std::vector<SourceTile*> ancestors;
	auto cursor_min = source.find_best_match(tileregion, &ancestors);
	target.adjust_resolution(cursor_min.get_resolution());
	// Query the tail pos
	auto shapecorner = tileregion;
	shapecorner.init_coord += target.tail_pos();
	auto cursor_max = source.find_best_match(shapecorner);

	return BlitterFromMTile<SourceTile, TargetTile>(cursor_min.now,
			ancestors,
			cursor_min.now,
			cursor_max.now,
			target);
}

#endif
