#ifndef ENTITY_H
#define ENTITY_H

#include "map.h"
#include "space.h"
#include "status.h"

typedef struct {
  int id;
  Space2d space;
  Vec2d velocity;
  Status status;
  Tile *hitTile;
  int onGround;
  double rotation;
} Entity;

void initEntity(Entity *entity);

void saveEntity(Entity *entity, const char *path);

int onGround(Entity *e);

void updateEntity(Entity *e);

#endif