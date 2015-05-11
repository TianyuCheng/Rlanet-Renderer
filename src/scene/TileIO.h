#ifndef TILE_IO_H
#define TILE_IO_H

#include <memory>
#include <vector>
#include <algorithm>
#include <functional>

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
struct TileIO {
	TileIO(Elem* buf, ssize_t linenelem, intptr_t stride, size_t nline)
		:buf_(buf),
		linenelem_(linenelem),
		stride_in_bytes_(stride),
		linepos_(0),
		gpos_(0),
		nline_(nline)
	{
	}

	bool is_valid() const { return buf_ != nullptr; }

	Elem* now() const
	{
		char* newloc = gpos_ + reinterpret_cast<char*>(buf_);
		return reinterpret_cast<Elem*>(newloc);
	}

	ssize_t nline() const { return nline_; }
	ssize_t nelem() const { return nline_ * linenelem_; }

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
	ssize_t nline_;
};

template<typename InElem,
	typename OutElem = InElem,
	typename Splicer = TransparentSplicer<InElem> >
class TileSplicer {
public:
	TileSplicer(TileIO<InElem>& in, TileIO<OutElem>& out)
		:in_(in), out_(out)
	{
	}

	ssize_t splice(ssize_t nelem)
	{
		if (!in_.is_valid() || !out_.is_valid())
			return -1;
		nelem = std::min(nelem, in_.nelem());
		nelem = std::min(nelem, out_.nelem());
		ssize_t ret = 0;
		while (nelem > 0) {
			InElem* ipos = in_.now();
			OutElem* opos = out_.now();

			ssize_t lineelem = std::min(in_.lineremain(), out_.lineremain());
			lineelem = std::min(nelem, lineelem);
			splicer_.ncopy(ipos, lineelem, opos);

			in_.seek_by_elem(lineelem);
			out_.seek_by_elem(lineelem);
			ret += nelem;
			nelem -= lineelem;
			if (nelem <= 0)
				break;
		}
		return ret;
	}
protected:
	TileIO<InElem>& in_;
	TileIO<OutElem>& out_;
	Splicer splicer_;
};

#endif
