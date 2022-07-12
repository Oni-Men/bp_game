#include "space.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>

void Move(Space2d *t, int x, int y) {
  LEFT(t) += x;
  BOTTOM(t) += y;
}

int IsPointHit(Space2d *t, int px, int py) {
  if (px < LEFT(t) || RIGHT(t) < px) {
    return false;
  }

  if (py < BOTTOM(t) || TOP(t) < py) {
    return false;
  }

  return true;
}

Space2dHitResult CheckBoxHit(Space2d *a, Space2d *b) {
  Space2dHitResult res;
  if (a == NULL || b == NULL) {
    res.isHit = false;
    return res;
  }
  int left = fmin(LEFT(a), LEFT(b));
  int right = fmax(RIGHT(a), RIGHT(b));
  int top = fmax(TOP(a), TOP(b));
  int bottom = fmin(BOTTOM(a), BOTTOM(b));

  int width = a->width + b->width;
  int height = a->height + b->height;

  res.a = a;
  res.b = b;
  res.hitX = (right - left) < width;
  res.hitY = (top - bottom) < height;
  res.isHit = res.hitX && res.hitY;
  return res;
}

int IsBoxHit(Space2d *a, Space2d *b) { return CheckBoxHit(a, b).isHit; };

Vec2d AddVec2d(Vec2d a, Vec2d b) {
  Vec2d res;
  res.x = a.x + b.x;
  res.y = a.y + b.y;
  return res;
}

Vec2d SubVec2d(Vec2d a, Vec2d b) {
  Vec2d res;
  res.x = a.x - b.x;
  res.y = a.y - b.y;
  return res;
}

Vec2d MutiplyVec2d(Vec2d a, double scale) {
  Vec2d res;
  res.x = a.x * scale;
  res.y = a.y * scale;
  return res;
}

double MagnitudeVec2d(Vec2d v) { return sqrt(v.x * v.x + v.y * v.y); }

Vec2d NormalizeVec2d(Vec2d a) {
  double mag = MagnitudeVec2d(a);
  return MutiplyVec2d(a, 1 / mag);
}

double DotVec2d(Vec2d a, Vec2d b) { return a.x * b.x + a.y * b.y; }

Space2d NewSpace(double x, double y, double w, double h) {
  Space2d space;
  space.posx = x;
  space.posy = y;
  space.width = w;
  space.height = h;
  return space;
}
