#ifndef ENTITY_H
#define ENTITY_H

#include "space.h"
#include "status.h"

typedef struct {
  int id;
  Space2d space;
  Vec2d velocity;
  Status status;
  double rotation;
} Entity;

void initEntity(Entity *entity);

#endif