#ifndef SUB_TILE_H
#define SUB_TILE_H

#include "Tile.h"
#include "TileIO.h"

template<typename TileInfo>
class SubTile {
	typedef typename TileInfo::TileElement Element;
public:
	SubTile(Element* elem, const Vec2D<int>& shape, size_t stride_in_elem)
		:elem_(elem), shape_(shape), stride_in_elem_(stride_in_elem)
	{
	}

	const Element& at(int x, int y)
	{
		return elem_[x * stride_in_elem_ + y];
	}

	size_t nelem() const { return shape_.x * shape_.y; }
	size_t lineelem() const { return shape_.y; }
	intptr_t stride_in_bytes() const { return stride_in_elem_ * sizeof(Element); }
	size_t nline() const { return shape_.x; }

	TileIO<Element> getio()
	{
		return TileIO<Element>(elem_,
				lineelem(),
				stride_in_bytes(),
				nline());
	}
private:
	Element* elem_;
	Vec2D<int> shape_;
	size_t stride_in_elem_;
};

#endif
