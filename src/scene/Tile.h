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
struct SquareTileShape {
	T x, y;
	T d;
	T res; // Resolution
};

template<typename T = double>
struct Vec2D {
	T x,y;
};

// We also need TileInfo concenpt for sampling purpose
struct TerrainTileInfo : public SquareTileShape<double> {
#if 0
	double theta_, dtheta_;
	double phi_, dphi_;
#endif
	TerrainTileShape(SquareTileShape<double>& shape)
		:SquareTileShape<double>(shape)
	{
		iD_ = std::round(d/res);
		nelem_ = iD_ * iD_;
	}
	typedef double TileElement; // Height
	typedef Vec2D Coordinate; // Coordinate
	typedef int TitleSeed; // Seed for noise generation

	ssize_t get_linear(const Coordinate& coord) const
	{
		auto icoord = get_block(coord);
		return icoord.x * iD_ + y;
	}

	Vec2D<int> get_block(const Coordinate& coord) const
	{
		Vec2D<int> ret;
		ret.x = (coord.x - x)/d;
		ret.y = (coord.y - y)/d;
		return ret;
	}

	size_t nelement() const { return nelem_; }
	size_t nline() const { return iD_; }
private:
	ssize_t iD_;
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
class MacroTile : public Tile<TileInfo> {
public:
	typedef unique_ptr<MacroTile<TileInfo>> (*ChildrenArrayPointer)[nchild][nchild];

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
	template<typename SecondTile>
	bool blit_to(const Coord& axesmins, const Coord& axesmaxs, SecondTile& sectile)
	{
		const auto& secshape = sectile->get_shape();
		typedef SecondTile::Element SecondElement;
		MacroTileBlitCursor blit_cursor =
			create_cursor(axesmins, axesmaxs, sectile);
		do {
			auto iblock = blit_cursor.current_iblock();
			auto oblock = blit_cursor.current_oblock();
			TileSplicer splicer(iblock, oblock);
			splicer.splice(oblock.nelem());
		} while (blit_cursor.next());
		return true;
	}
protected:
	MacroTile<TileInfo>* parent_;
	unique_ptr<ChildrenArrayPointer> pchildren_array_;
	std::reference_wrapper<ChildrenArrayPointer> children_;
};

#endif
