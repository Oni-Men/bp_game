/**
 * @file button.h
 * @brief ボタンの構造体、制御、描画に関する処理
 * @copyright Copyright (c) 2022
 */

#include "space.h"

typedef struct {
  Space2d space;
  int buttonId;
  int background;
  int color;
  int fontType;
  double fontSize;
  const char *text;
} Button;

typedef struct {
  int size;
  Button *list;
} ButtonList;

void loadButton(const char *path);

void renderAllButton(int layer, Button buttons[], int n);

void renderButton(int layer, Button *button);

Button NewButton(int id, int x, int y, int w, int h, const char *str);

int getHoveredButton(Button buttons[], int n, int mousex, int mousey);
