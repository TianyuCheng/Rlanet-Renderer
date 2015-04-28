#ifndef TILE_H
#define TILE_H

#include <memory>
#include <vector>
#include <functional>

#include "Geo.h"

using std::unique_ptr;
using std::vector;

template<typename Elem>
struct TransparentSplicer {
	void ncopy(Elem* src, size_t n, Elem* dst)
	{
		std::copy_n(src, n, dst);
	}
};

template<typename Elem>
//, typename OutElem = InElem, typename Reader = TransparentReader<Elem> >
struct TileIO {
	TileIO(Elem* buf, ssize_t linenelem, intptr_t stride, size_t nline)
		:buf_(buf),
		linenelem_(linenelem),
		stride_in_bytes_(stride),
		linepos_(0),
		gpos_(0)
	{
	}

	bool is_valid() const { return buf_ != nullptr; }

	Elem* now() const
	{
		void* newloc = gpos_ + static_cast<void*>(buf_);
		return static_cast<Elem*>(newloc);
	}

	size_t nline() const { return nline_; }
	size_t nelem() const { return nline_ * linenelem_; }

	ssize_t lineremain() const
	{
		return linenelem_ - linepos_;
	}

	void seek_by_line(ssize_t nline)
	{
		gpos_ += nline * stride_in_bytes_;
	}

	void seek_by_elem(ssize_t nelem)
	{
		if (nelem == lineremain()) { // Most common case
			seek_by_line(1);
		} else if (nelem < lineremain()) {
			linepos_ += nelem;
			gpos_ += nelem * sizeof(Elem);
		} else {
			nelem += linepos_;
			// Seek to the start of the line
			gpos_ -= linepos_ * sizeof(Elem);
			linepos_ = 0;
			// Move some lines
			seek_by_line(nelem/linenelem_);
			// And move by the number of remaining elements
			seek_by_elem(nelem&linenelem_);
		}
	}
private:
	Elem* buf_;
	intptr_t linepos_;
	intptr_t gpos_;
	ssize_t linenelem_;
	intptr_t stride_in_bytes_;
	size_t nline_;
};

template<typename InElem,
	typename OutElem = InElem,
	typename Splicer = TransparentSplicer<InElem, OutElem> >
class TileSplicer {
public:
	TileSplicer(TileIO<InElem>& in, TileIO<OutElem>& out)
		:in_(in), out_(out)
	{
	}

	ssize_t splice(ssize_t nelem)
	{
		if (!in.is_valid() || !out.is_valid())
			return -1;
		nelem = std::min(nelem, in.nelem());
		nelem = std::min(nelem, out.nelem());
		ssize_t ret = 0;
		while (nelem > 0) {
			InElem* ipos = in.now();
			OutElem* opos = out.now();

			ssize_t lineelem = std::min(in.lineremain(), out.lineremain());
			lineelem = std::min(nelem, lineelem);
			splicer_.ncopy(ipos, lineelem, opos);

			ipos.seek_by_elem(lineelem);
			opos.seek_by_elem(lineelem);
			ret += nelem;
			lineelem -= nelem;
			if (nelem <= 0)
				break;
		}
		return ret;
	}
protected:
	TileIO<InElem>& in_;
	TileIO<OutElem>& out_;
	Splicer<InElem, OutElem> splicer_;
}

template<typename T>
struct TileShape {
	T x, y;
	T dx, dy;
	T res; // Resolution
};

template<typename T = double>
struct Vec2D {
	T x,y;

	bool wrap(T maxx, T maxy); // TODO: return true if need wrap x to [0, maxx) (similar to y)
};

// We also need TileInfo concenpt for sampling purpose
struct TerrainTileInfo : public TileShape<double> {
#if 0
	double theta_, dtheta_;
	double phi_, dphi_;
#endif
	TerrainTileShape(SquareTileShape<double>& shape)
		:SquareTileShape<double>(shape)
	{
		ix_ = std::round(dx/res);
		iy_ = std::round(dy/res);
		nelem_ = ix_ * iy_;
	}
	typedef double TileElement; // Height
	typedef Vec2D Coordinate; // Coordinate
	typedef int TitleSeed; // Seed for noise generation

	ssize_t get_linear(const Coordinate& coord) const
	{
		auto icoord = get_block(coord);
		return icoord.x * iy_ + y;
	}

	Vec2D<int> get_block(const Coordinate& coord) const
	{
		Vec2D<int> ret;
		ret.x = (coord.x - x)/d;
		ret.y = (coord.y - y)/d;
		return ret;
	}

	size_t nelement() const { return nelem_; }
	size_t nline() const { return ix_; }
private:
	ssize_t ix_, iy_;
	size_t nelem_;
};

template<typename TileInfo>
class Tile {
pubic:
	typedef typename TileInfo::Coordinate Coord;
	typedef typename TileInfo::TileSeed Seed;
	typedef typename TileInfo::TileElement Element;

	Tile() = delete;
	Tile(const TileShape& shape, const Seed& seed)
		:shape_(shape), seed_(seed)
	{
	}
	virtual ~Tile() {}

	bool write(const Coord& coord, const Element& elem)
	{
		ssize_t loc = shape_.get_linear(coord);
		if (loc < 0)
			return false;
		alloc_memory();
		elems_[loc] = elem;
		return true;
	}

	bool read(const Coord& coord, const Element& elem) const
	{
		ssize_t loc = shape_.get_linear(coord);
		if (loc < 0) {
			return false;
		}
		gen();
		elem = elems_[loc];
	}

	TileIO<Element> get_ioblock(const Coord& axesmins,
			const Coord& axesmaxs,
			int LODLevel)
	{
		alloc_memory();

		int istart, iend, jstart, jend;
		ssize_t start = shape_.get_linear(axesmins);
		auto imins = shape_.get_block(axesmins);
		auto imaxs = shape_.get_block(axesmaxs);
		if (imins.x < 0 || imaxs.x < 0)
			return TileIO<Element>(nullptr, 0, 0);
		ssize_t lineelem = imaxs.y - imins.y;
		intptr_t stride = sizeof(Elem) * lineelem;
		Elem* buf = &elems[start];
		return TileIO<Element>(buf, lineelem, stride, imaxs.x - imins.x);
	}

	const TileShape& get_shape_info() const { return shape_; }
protected:
	TileInfo shape_;
	Seed seed_;
	mutable std::vector<Element> elems_;

	void alloc_memory()
	{
		if (elems_.empty()) {
			size_t nelem = shape_->nelement();
			elems_.resize(nelem);
		}
	}

	void gen()
	{
		if (elems_.empty()) {
			alloc_memory();
			TileInfo::Generator generator(seed_);
			generator.gen(elems_);
		}
	}
};

/*
 * Application of class template Tile
 */
typedef Tile<TerrainTileInfo> TerrainTile;

template<typename TileInfo, int nchild>
class MacroTile;

struct TileSetCursorPosition : public std::vector<Vec2D<int>> {
};

inline bool operator<(const TileSetCursorPosition& lhs, const TileSetCursorPosition& rhs)
{
	if (lhs.size() != rhs.size())
		return false;
	auto iter1 = lhs.begin();
	auto iter2 = rhs.begin();
	while (iter1 != lhs.end() && *iter1 == *iter2) {
		iter1++;
		iter2++;
	}
	return *iter1 < *iter2;
}

template<typename TileInfo, int nchild>
struct TileHierarchyCursor {
	typedef MacroTile<TileInfo, nchild> MTile;
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
				ancestors[iter + 1] = ancestors[iter].child(now[iter+1]);
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
template<typename TileInfo, int nchild, typename SecondTile>
class MacroTileBlitCursor : public TileHierarchyCursor<TileInfo, nchild> {
	typedef TileSetCursor<TileInfo, nchild> TileCursor;
	MacroTileBlitCursor(const TileSetCursorPosition& start,
			const std::vector<MacroTile<TileInfo>*>&,
			const TileSetCursorPosition& _mins,
			const TileSetCursorPosition& _maxs,
			SecondTile &sectile,
			)
		:TileHierarchyCursor<TileInfo, nchild>(start, ancestors),
		mins(_mins), maxs(_maxs)
	{
	}

	TileSetCursorPosition mins, maxs; // Bounds, used by CR LF and successor
	SecondTile &sectile,

	/*
 	 * Time to learn the original meaning of these two words
	 * CR: move the carriage to the start of CURRENT line
	 * LF: move the carriage to the next line
	 *
	 * Thus, here we require CR+LF to get to the start of new line.
 	 */
	void carriage_return();
	bool line_feed();
	// successor: move the cursor to the next tile in the same line
	bool successor();

	TileIO<TileInfo::TileElement> current_iblock();
	TileIO<SecondTile::Element> current_oblock();

	// TODO
	bool next();
};

template<typename TileInfo, int nchild>
class MacroTile : public Tile<TileInfo> {
public:
	typedef MacroTile<TileInfo, nchild> MTile;
	typedef TileSetCursor<TileInfo, int nchild> Cursor;
	typedef std::vector<MTile*> Ancestors;
	typedef unique_ptr<MTile> (*ChildrenArrayPointer)[nchild][nchild];

	bool is_leaf() const { return pchildren_array_.get() == nullptr; }
	void init_children()
	{
		ChildrenArrayPointer childrenlist = new ChildrenArrayPointer;
		pchildren_array_.reset(childrenlist);
		children_ = std::ref(*childrenlist);
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

	template<typename SecondTile>
	MacroTileBlitCursor<MTile, SecondTile>
	create_blit_cursor(SecondTile& sectile)
	{
		const auto& tileregion = sectile.get_shape_info();
		std::vector<MTile> ancestors;
		auto cursor_min = find_best_match(tileregion, &ancestors);
		auto shapecorner = secshape;
		shapecorner.x += secshape.d;
		shapecorner.y += secshape.d;
		auto cursor_max = find_best_match(secshape);

		return MacroTileBlitCursor<TileInfo, nchild, SecondTile> ret(cursor_min,
				ancestors,
				cursor_min,
				cursor_max,
				sectile
				);
	}

	// TODO
	TileSetCursor<TileInfo> find_best_match(const TileInfo& area,
			Ancestors* pancestors = nullptr)
	{
	}

	// TODO
	MTile* child(const Vec2D<int>&);
protected:
	MTile* parent_;
	unique_ptr<ChildrenArrayPointer> pchildren_array_;
	std::reference_wrapper<ChildrenArrayPointer> children_;
};

#endif
