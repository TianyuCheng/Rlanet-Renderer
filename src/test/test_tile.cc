#include "MacroTile.h"

const double Equator = 4.00e7;
const int nchild = 32;

int main()
{
#if 0
	TileShape<double> earthsize;
	earthsize.init_coord = Vec2D<double>(0.0, 0.0);
	earthsize.shape = Vec2D<double>(Equator, Equator);
	earthsize.res = Equator/nchild;

	TerrainTileInfo meta(earthsize);
	Zearth earth(meta, 0);

	TileShape<double> target;
	target.init_coord = Vec2D<double>(1e6, 1e6);
	target.shape = Vec2D<double>(1e6, 1e6);
	target.res = 1e3;
	fprintf(stderr, "Target res %f\n", target.res);
	TerrainTileInfo query(target);
	fprintf(stderr, "Query res %f\n", query.get_resolution(0));
	Tile<TerrainTileInfo> target_tile(target, 0);
	
	earth.blit_to(target_tile);
#else
	Zearth earth(TerrainTileInfo(TileShape<float>({0.0f, 0.0f}, {1024.0f, 1024.0f}, 1.0)), 0);
	Tile<TerrainTileInfo> target_tile(TerrainTileInfo(TileShape<float>({512.0f, 512.0f}, {512.0f, 512.0f}, 0.5)), 0);
	earth.blit_to(target_tile);
	fprintf(stderr, "=================================================================\n");

	Tile<TerrainTileInfo> target_tile2(TerrainTileInfo(TileShape<float>({0.0f, 0.0f}, {512.0f, 512.0f}, 2.0)), 0);
	earth.blit_to(target_tile2);
#endif
	return 0;
}
