#include "button.h"

#include <handy.h>
#include <stdio.h>
#include <stdlib.h>

#include "space.h"

void renderAllButton(int layer, ButtonList *list) {
  for (int i = 0; i < list->size; i++) {
    renderButton(layer, &list->button_arr[i]);
  }
}

void renderButton(int layer, Button *button) {
  int x = button->space.posx;
  int y = button->space.posy;
  int w = button->space.width;
  int h = button->space.height;

  HgWSetFillColor(layer, button->background);
  HgWBoxFill(layer, x, y, w, h, 0);

  HgWSetFont(layer, HG_GB, 24.0);
  HgWSetColor(layer, button->color);

  double tw, th;
  HgWTextSize(layer, &tw, &th, button->text);
  HgWText(layer, x + (w - tw) / 2, y + (h - th) / 2, button->text);
}

ButtonList *NewButtonList(int cap) {
  ButtonList *list = malloc(sizeof(ButtonList));
  list->size = 0;
  list->cap = cap;
  return list;
}

int AddButton(ButtonList *list, int x, int y, int w, int h, const char *str) {
  Button btn;
  btn.buttonId = list->size;
  btn.space = NewSpace(x, y, w, h);
  btn.text = str;
  btn.background = HG_BLACK;
  btn.color = HG_WHITE;
  btn.fontType = 0x42;
  btn.fontSize = 16.0;
  list->button_arr[list->size] = btn;
  list->size++;
  return btn.buttonId;
}

void ClearButtons(ButtonList *list) { list->size = 0; }

int getHoveredButton(ButtonList *list, int mousex, int mousey) {
  int n = list->size;
  Button *buttons = list->button_arr;
  for (int i = 0; i < n; i++) {
    if (IsPointHit(&(buttons[i].space), mousex, mousey)) {
      return i;
    }
  }
  return -1;
}
