#include "button.h"

#include <handy.h>
#include <stdio.h>
#include <stdlib.h>

#include "space.h"

ButtonList *loadButtons(const char *path) {
  FILE *f;
  ButtonList *buttons = malloc(sizeof(ButtonList));

  if ((f = fopen(path, "r")) == NULL) {
    return NULL;
  }

  fscanf(f, "%d", &buttons.size);
  buttons.list = malloc(buttons.size * sizeof(Button));

  char buf[1024];
  for (int i = 0; i < buttons.size; i++) {
    fscanf(f, "%s", &buf[0]);
  }
}

void renderAllButton(int layer, Button *buttons, int n) {
  for (int i = 0; i < n; i++) {
    renderButton(layer, &buttons[i]);
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

Button NewButton(int id, int x, int y, int w, int h, const char *str) {
  Button btn;
  btn.buttonId = id;
  btn.space = NewSpace(x, y, w, h);
  btn.text = str;
  btn.background = HG_BLACK;
  btn.color = HG_WHITE;
  btn.fontType = 0x42;
  btn.fontSize = 16.0;
  return btn;
}

int getHoveredButton(Button buttons[], int n, int mousex, int mousey) {
  for (int i = 0; i < n; i++) {
    if (IsPointHit(&(buttons[i].space), mousex, mousey)) {
      return i;
    }
  }
  return -1;
}
