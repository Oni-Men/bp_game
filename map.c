#include "map.h"

#include <handy.h>
#include <stdio.h>
#include <stdlib.h>

Map *loadMap(const char *path) {
  printf("Start loading map...\n");
  FILE *f;
  Map *map = malloc(sizeof(Map));
  Tile *tile;

  if ((f = fopen(path, "r")) == NULL) {
    return NULL;
  }

  fscanf(f, "%32s", &map->name[0]);
  fscanf(f, "%d %d", &map->width, &map->height);

  int memSize = sizeof(Tile) * map->width * map->height;
  map->tiles = malloc(memSize);

  printf("Map loaded: %s\n", map->name);
  printf("Map size: %d, %d\n", map->width, map->height);
  printf("Loader allocated memory for Map Data: %d\n", memSize);

  int id;
  char line[map->width + 1];
  for (int y = map->height - 1; y >= 0; y--) {
    fscanf(f, "%s", &line[0]);
    for (int x = 0; x < map->width; x++) {
      id = line[x] - 48;
      if (id < 0) {
        continue;
      }
      tile = &map->tiles[y * map->width + x];
      tile->x = x;
      tile->y = y;
      tile->type = id;
      tile->space.posx = x * TILE_SIZE;
      tile->space.posy = y * TILE_SIZE;
      tile->space.width = TILE_SIZE;
      tile->space.height = TILE_SIZE;
    }
  }

  fclose(f);

  map->textures[TILE_AIR] = 0;
  map->textures[TILE_GRASS] = HgImageLoad("./images/grass.png");
  map->textures[TILE_STONE] = 0;
  return map;
}

void unloadMap(Map *map) { free(map); }

void RenderMap(int layer, Map *map, Vec2d *cameraPos) {
  Tile *tile;
  int tex;

  Tile *p = (Tile *)map->tiles;

  double width, height;
  HgGetSize(&width, &height);

  Space2d renderSpace, tileSpace;
  renderSpace.posx = 0;
  renderSpace.posy = 0;
  renderSpace.width = width;
  renderSpace.height = height;

  int count = 0;
  for (int y = 0; y < map->height; y++) {
    for (int x = 0; x < map->width; x++) {
      tile = (p + (y * map->width) + x);
      tex = map->textures[tile->type];
      if (tex == 0) {
        continue;
      }
      count += renderTile(layer, tex, tile, cameraPos, width, height);
    }
  }
  // HgWSetColor(layer, HG_GREEN);
  // HgWSetFont(layer, HG_GB, 36);
  // HgWText(layer, 100, 100, "%d", count);
}

int renderTile(int layer, int tex, Tile *tile, Vec2d *cameraPos, double width,
               double height) {
  int x = (tile->x) * TILE_SIZE - cameraPos->x + width / 2;
  int y = (tile->y) * TILE_SIZE - cameraPos->y + height / 2;

  // If the tile were out of window. do not render the tile.
  if (x < -TILE_SIZE || x > width + TILE_SIZE) return 0;
  if (y < -TILE_SIZE || y > height + TILE_SIZE) return 0;

  HgWImageDrawRect(layer, x, y, TILE_SIZE, TILE_SIZE, tex, 0, 0, 16, 16);
  return 1;
}

/**
 * @brief Get the Tile At object. Returns NULL if the given (x, y) were out of
 * the map.
 *
 * @param map
 * @param x
 * @param y
 * @return Tile*
 */
Tile *getTileAt(Map *map, int x, int y) {
  int tilex = x / TILE_SIZE;
  int tiley = y / TILE_SIZE;

  if (tiley < 0 || tilex >= map->width) {
    return NULL;
  }
  if (tiley < 0 || tiley >= map->height) {
    return NULL;
  }

  return &map->tiles[tiley * map->width + tilex];
}