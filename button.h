#ifndef BUTTON_H
#define BUTTON_H
/**
 * @file button.h
 * @brief ボタンの構造体、制御、描画に関する処理
 * @copyright Copyright (c) 2022
 */

#include "space.h"

/**
 * @brief ボタンを表現するデータ構造
 *
 */
typedef struct {
  int id;

  // カーソルがボタン上に入ってから経過した時間
  int timeHovered;

  // ボタン上でマウスをクリックしてから経過した時間。
  int timePressed;

  Space2d space;
  const char *text;
} Button;

#define L ButtonList
typedef struct L *L;
struct L {
  Button *val;
  L next;
};

typedef void (*ButtonRender)(int id, Button *b);

/**
 * @brief ボタンリストを作成する
 *
 * @return ButtonList*
 */
L NewButtonList();

L AddButton(L list, int id, Space2d box, const char *str);

void RenderAllButton(int layer, L list, ButtonRender renderer);

Button *GetHoveredButton(L list, int mousex, int mousey);

#undef L
#endif