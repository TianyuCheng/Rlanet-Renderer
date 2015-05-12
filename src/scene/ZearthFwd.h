#ifndef ZEARTH_FWD_H
#define ZEARTH_FWD_H

#include "Geo.h"

template<typename T> struct TileMeta;
typedef TileMeta<GeoInfo> TerrainTileInfo;

template<typename TileInfo, int _nchild> class MacroTile;
typedef MacroTile<TerrainTileInfo, 16> Zearth; // The Earth

#endif
