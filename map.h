#ifndef MAP_H
#define MAP_H

#include "space.h"

#define TILE_SIZE (64)

#define TILE_AIR 0
#define TILE_GRASS 1
#define TILE_STONE 2

typedef struct {
  double x;
  double y;
  int type;
  Space2d space;
} Tile;

typedef struct {
  char name[32];
  int textures[3];
  int width;
  int height;
  Tile *tiles;
} Map;

Map *loadMap(const char *path);
void unloadMap(Map *map);
void renderMap(int layer, Map *map, Vec2d *cameraPos);
int renderTile(int layer, int tex, Tile *tile, Vec2d *cameraPos, double width,
               double height);

/**
 * @brief Get the Tile At specified point(x, y).
 *
 * @param map*
 * @param x
 * @param y
 * @return Tile*
 */
Tile *getTileAt(Map *map, int x, int y);

#endif