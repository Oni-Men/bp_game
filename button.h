#ifndef BUTTON_H
#define BUTTON_H
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

  /**  @brief ボタンリストの上限 */
  int cap;
  Button *button_arr;
} ButtonList;

/**
 * @brief 与えられたcapを上限とするボタンリストを作成する
 *
 * @param cap
 * @return ButtonList*
 */
ButtonList *NewButtonList(int cap);

/**
 * @brief 与えられたボタンリストに、与えられたパラメータで作ったボタンを追加する
 *
 * @param list
 * @param x
 * @param y
 * @param w
 * @param h
 * @param str
 * @return int
 */
int AddButton(ButtonList *list, int x, int y, int w, int h, const char *str);

void renderAllButton(int layer, ButtonList *list);

void renderButton(int layer, Button *button);

int getHoveredButton(ButtonList *list, int mousex, int mousey);

#endif