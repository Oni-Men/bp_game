#include <math.h>

#ifndef SPACE_H
#define SPACE_H

#define LEFT(t) (t->posx)
#define RIGHT(t) (t->posx + t->width)
#define BOTTOM(t) (t->posy)
#define TOP(t) (t->posy + t->height)
#define CENTER_X(t) (t->posx + t->width / 2)
#define CENTER_Y(t) (t->posy + t->height / 2)

typedef struct {
  double x;
  double y;
} Vec2d;

typedef struct {
  double posx;
  double posy;
  double width;
  double height;
} Space2d;

typedef struct {
  Space2d *a;
  Space2d *b;
  int isHit;
  int hitX;
  int hitY;
} Space2dHitResult;

int Translate(Space2d *a, Vec2d *v);

void Move(Space2d *a, int x, int y);

int IsPointHit(Space2d *t, int px, int py);

Space2dHitResult CheckBoxHit(Space2d *a, Space2d *b);

int IsBoxHit(Space2d *a, Space2d *b);

Vec2d AddVec2d(Vec2d a, Vec2d b);

Vec2d SubVec2d(Vec2d a, Vec2d b);

Vec2d MutiplyVec2d(Vec2d a, double scale);

double MagnitudeVec2d(Vec2d v);

Vec2d NormalizeVec2d(Vec2d a);

double DotVec2d(Vec2d a, Vec2d b);

Space2d NewSpace(double x, double y, double w, double h);

#endif