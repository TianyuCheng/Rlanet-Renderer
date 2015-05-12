#ifndef TILE_H
#define TILE_H

#include <memory>
#include <vector>
#include <functional>

#define TILE_DEBUG 0

#include "Vec2D.h"
#include "TileInfo.h"
#include "TileIO.h"

using std::unique_ptr;
using std::vector;

template<typename TileInfo>
class Tile {
public:
	typedef typename TileInfo::Coordinate Coord;
	typedef typename TileInfo::TileSeed Seed;
	typedef typename TileInfo::FloatType FloatType;
	typedef typename TileInfo::TileElement Element;

	Tile() = delete;
	Tile(const TileInfo& shape, const Seed& seed)
		:shape_(shape), seed_(seed)
	{
	}
	virtual ~Tile() {}

	TileIO<Element> get_ioblock(const Coord& axesmins,
			const Coord& axesmaxs,
			int LODLevel)
	{
		TileInfo lodshape = shape_;
		lodshape.recalibre(get_resolution(LODLevel));

		int istart, iend, jstart, jend;
		ssize_t start = lodshape.get_linear(axesmins);
		auto imins = lodshape.get_block(axesmins);
		auto imaxs = lodshape.get_block(axesmaxs);
		if (imins.x < 0 || imaxs.x < 0)
			return TileIO<Element>(nullptr, 0, 0, 0);
		size_t block_lineelem = imaxs.y - imins.y;
		intptr_t stride;
		Element* buf = access_lod(LODLevel, start, &stride);
		stride *= sizeof(Element);

		fprintf(stderr, "\t%s, accessing LOD %d with size %lu\n",
				__func__,
				LODLevel, access_lod_vec(LODLevel)->size());

		return TileIO<Element>(buf,
				block_lineelem,
				stride,
				imaxs.x - imins.x);
	}

	Element* access_lod(int LODLevel, ssize_t offset, intptr_t* pstride)
	{
		gen(LODLevel);
		*pstride = shape_.nelem_in_line() >> LODLevel;
		return access_lod_vec(LODLevel)->data() + offset;
	}

	const TileInfo& get_shape_info() const { return shape_; }
	Coord init_pos() const { return shape_.init_pos(); }
	Coord tail_pos() const { return shape_.tail_pos(); }
	FloatType get_resolution(int LODLevel) const { return shape_.get_resolution(LODLevel); }

	void adjust_resolution(double newres)
	{
		elems_.clear();
		shape_.recalibre(newres);
		alloc_memory();
	}

	void relocate(const Coord& coord)
	{
		shape_.init_coord = coord;
	}
protected:
	TileInfo shape_;
	Seed seed_;
	std::vector<Seed> lodseeds_;
	mutable std::vector<Element> elems_; // LOD level 0
	mutable std::vector<std::vector<Element>> lods_; // LOD level from 1 to nchild -1
	mutable std::vector<Element> pattern_; // LOD level nchild

	void alloc_memory()
	{
		if (elems_.empty()) {
			size_t nelem = shape_.nelement();
			elems_.resize(nelem);
		}
	}

	std::vector<Element>* access_lod_vec(int LODLevel)
	{
		if (LODLevel == 0) {
			return &elems_;
		}
		if (LODLevel <= lods_.size()) {
			return &lods_[LODLevel-1];
		}
		return &pattern_;
	}

	void gen_base()
	{
		if (!elems_.empty())
			return ;
		alloc_memory();
		typename TileInfo::Generator generator(seed_);
		generator.gen_terrain(elems_, shape_.nline(), shape_.nelem_in_line());
	}

	void gen(int LODLevel = 0)
	{
		// Allocate the base memory
		if (LODLevel == 0) {
			gen_base();
			return ;
		}

		// Allocate LOD pointers if necessary
		if (lods_.empty()) {
			size_t total_level;
			if (pattern_.empty())
				total_level = LODLevel;
			else
				total_level = lround(log2(shape_.nelement()/pattern_.size())/2) - 1;
			lods_.resize(total_level);

			lodseeds_.resize(total_level+1);
			typename TileInfo::Generator generator(seed_);
			generator.gen_lodseeds(lodseeds_);
		}
		if (pattern_.empty()) { // Downsample
			// Generate the base layer
			gen_base();
			for(int i = 1; i <= LODLevel; i++) {
				downsample(shape_.nline() >> (i-1),
						*access_lod_vec(i-1),
						*access_lod_vec(i));
			}
			return ;
		}
		fprintf(stderr, "Tile %p is generating LOD from Pattern %p\n",
				this, pattern_.data());
		typename TileInfo::Generator generator(seed_);
		for(int i = lods_.size() + 1; i > LODLevel; i--) {
			auto pattern = access_lod_vec(i);
			auto higher = access_lod_vec(i-1);
			generator.gen_from_pattern(
				shape_.nline() >> (i-1),
				shape_.nline() >> (i),
				*higher, // LOD to generate
				*pattern,
				lodseeds_[i]
				); // pattern
			fprintf(stderr, "\t\tGenerating LOD %d from LOD %d"
					", %lu <- %lu "
					"Seed: %lu\n"
					,i-1, i,
					higher->size(), pattern->size(),
					lodseeds_[i-1]
					);
		}
	}
};

/*
 * Application of class template Tile
 */
typedef Tile<TerrainTileInfo> TerrainTile;

#endif
