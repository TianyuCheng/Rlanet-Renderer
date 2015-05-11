#ifndef ZEARTH_FWD_H
#define ZEARTH_FWD_H

struct TerrainTileInfo;

template<typename TileInfo, int _nchild> class MacroTile;
typedef MacroTile<TerrainTileInfo, 16> Zearth; // The Earth

#endif
