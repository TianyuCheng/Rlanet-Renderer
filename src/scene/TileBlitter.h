#ifndef TILE_BLITTER_H
#define TILE_BLITTER_H

#include "MacroTile.h"

struct TileSetCursorPosition : public std::vector<Vec2D<int>> {
	void tell(bool linefeed = true) const
	{
		fprintf(stderr, "%s: ", __func__);
		for(const auto& iter : *this)
			fprintf(stderr, "(%3d, %3d) ", iter.x, iter.y);
		if (linefeed)
			fprintf(stderr, "\n");
	}

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

inline bool collide_le(const TileSetCursorPosition& lhs, const TileSetCursorPosition& rhs)
{
	if (lhs.size() != rhs.size())
		return false;
	size_t i = 0;
	for(; i < lhs.size() && lhs[i] == rhs[i]; i++)
		;
	bool ret = collide_le(lhs[i], rhs[i]);
	if (ret) {
		fprintf(stderr, "COLLIDE\n");
		lhs.tell();
		rhs.tell();
	}
	return ret;
}

inline bool collide_l(const TileSetCursorPosition& lhs, const TileSetCursorPosition& rhs)
{
	if (lhs.size() != rhs.size())
		return false;
	size_t i = 0;
	for(; i < lhs.size() && lhs[i] == rhs[i]; i++)
		;
	bool ret = collide_l(lhs[i], rhs[i]);
	if (ret) {
		fprintf(stderr, "COLLIDE\n");
		lhs.tell();
		rhs.tell();
	}
	return ret;
}


inline bool operator<=(const TileSetCursorPosition& lhs, const TileSetCursorPosition& rhs)
{
	if (lhs.size() != rhs.size())
		return false;
#if TILE_DEBUG
	fprintf(stderr, "Comparing "); lhs.tell(false); fprintf(stderr, "\tand\t"); rhs.tell();
#endif
	size_t i = 0;
	for(; i < lhs.size() && lhs[i] == rhs[i]; i++)
		;
#if TILE_DEBUG
	fprintf(stderr, "\tNeed to Compare (%d, %d) and (%d, %d)\n", lhs[i].x, lhs[i].y,
			rhs[i].x, rhs[i].y);
#endif
	return lhs[i] < rhs[i];
}

template<typename TileInfo, int nchild>
class MacroTile;

template<typename MTile>
struct TileHierarchyCursor {
	static constexpr int nchild = MTile::nchild;
	typedef typename MTile::Coord Coord;

	TileHierarchyCursor(const TileSetCursorPosition& pos,
			const std::vector<MTile*>& anc, int lod = 0)
		:now(pos), ancestors(anc), LODLevel(lod)
	{
	}

	TileHierarchyCursor(const TileHierarchyCursor& cursor)
	{
		*this = cursor;
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
#if TILE_DEBUG
		fprintf(stderr, "%s before move (%d, %d): ", __func__, dx, dy); this->now.tell();
#endif

		bool ret = false;
		while (iter >= 0) {
			now[iter] += dvec; // Move cursor at current tile level
			if (!now[iter].wrap(nchild, nchild))
				ret = true; // No need to move higher level cursor
			if (iter >= 0) {
				// Maintain ancestors
				fprintf(stderr, "Move to MTile %p (%d, %d)\n",
						ancestors.back(),
						now[iter].x,
						now[iter].y
						);
				ancestors[iter + 1] = ancestors[iter]->child(now[iter]);
			}
			iter--;
			if (ret) { // Return if done
#if TILE_DEBUG
				fprintf(stderr, "\t after move: "); this->now.tell();
#endif
				return ret;
			}
			// Otherwise proceed to higher level
		}
#if TILE_DEBUG
		fprintf(stderr, "\t [OOB!] after move: "); this->now.tell();
#endif
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

	BlitterFromMTile(const TileCursor& start,
			const std::vector<SourceTile*>& anc,
			const TileSetCursorPosition& _mins,
			const TileSetCursorPosition& _maxs,
			TargetTile &_sect
			)
		:TileHierarchyCursor<SourceTile>(start),
		mins(_mins), maxs(_maxs),
		write_pos_(0,0), sectile(_sect)
	{
		fprintf(stderr, "%s %p, pixels (%lu, %lu)\n", __func__, this,
				_sect.get_shape_info().nline(),
				_sect.get_shape_info().nelement()/_sect.get_shape_info().nline()
				);
		fprintf(stderr, "\tmaxs\t"); maxs.tell();
		fprintf(stderr, "\tmins\t"); mins.tell();
		fprintf(stderr, "\tnow\t"); this->now.tell();
		fprintf(stderr, "\tLOD%d\n", this->LODLevel);
	}

	TileSetCursorPosition mins, maxs; // Bounds, used by CR LF and successor
	TargetTile &sectile;

	bool oob() const
	{
#if TILE_DEBUG
		fprintf(stderr, "Checkint OOB %p\n", this);
		fprintf(stderr, "\tmaxs\t"); maxs.tell();
		fprintf(stderr, "\tmins\t"); mins.tell();
		fprintf(stderr, "\tnow\t"); this->now.tell();
#endif
		if (collide_le(maxs, this->now) || collide_l(this->now, mins)) {
			fprintf(stderr, "\n\t OOB at "); this->now.tell();
			return true;
		}
		return false;
	}

	void tell(bool linefeed = true)
	{
		fprintf(stderr, "BlitterFromMTile current: "); this->now.tell(linefeed);
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
		fprintf(stderr, "%s to: ",__func__); this->now.tell();
		write_pos_.y = 0;
	}

	bool line_feed()
	{
#if TILE_DEBUG
		fprintf(stderr, "%s\n", __func__);
#endif
		bool ret = this->down();
		write_pos_.x++;
		return ret && !oob();
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
#if TILE_DEBUG
		fprintf(stderr, "iblock (%f, %f) - (%f, %f)\n",
				min.x, min.y,
				max.x, max.y);
#endif
		return now->get_ioblock(min, max, this->LODLevel);
	}

	TileIO<TargetElement> current_oblock()
	{
		Coord min, max;
		get_blitting_region(min, max);
#if TILE_DEBUG
		fprintf(stderr, "oblock (%f, %f) - (%f, %f)\n",
				min.x, min.y,
				max.x, max.y);
#endif
		return sectile.get_ioblock(min, max, 0);
	}

	bool next()
	{
#if TILE_DEBUG_
		fprintf(stderr, "Tile pos (%d, %d)\n", write_pos_.x, write_pos_.y);
#endif
		bool notoverflow = this->right(); // Move one step right
		if (notoverflow && !oob())
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
	fprintf(stderr, "shape_res %f\n", tileregion.get_resolution(0));
	std::vector<SourceTile*> ancestors;
	auto cursor_min = source.find_best_match(tileregion, &ancestors);
	fprintf(stderr, "cursor_min "); cursor_min.now.tell(false);
	fprintf(stderr, "\t LOD %d\n", cursor_min.LODLevel);
	// Query the tail pos
	auto shapecorner = tileregion;
	shapecorner.init_coord += target.tail_pos();
	auto cursor_max = source.find_best_match(shapecorner);
	fprintf(stderr, "cursor_max "); cursor_max.now.tell();
	fprintf(stderr, "Max resolution: %f\n", shapecorner.get_resolution(0));
	target.adjust_resolution(cursor_min.get_resolution());
	fprintf(stderr, "adjust input resolution to %f\n", target.get_resolution(0));

	return BlitterFromMTile<SourceTile, TargetTile>(cursor_min,
			ancestors,
			cursor_min.now,
			cursor_max.now,
			target);
}

#endif
