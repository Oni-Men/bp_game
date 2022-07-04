#include "button.h"

#include <handy.h>
#include <stdio.h>
#include <stdlib.h>

#include "space.h"

#define L ButtonList

void RenderAllButton(int layer, L list, ButtonRender renderButton) {
  if (renderButton == NULL) {
    return;
  }

  do {
    renderButton(layer, list->val);
    list = list->next;
  } while (list);
}

L NewButtonList() {
  L list;
  list = (L)malloc(sizeof(L));
  list->next = NULL;
  return list;
}

L AddButton(L list, int id, Space2d box, const char *str) {
  L newList = (L)malloc(sizeof(L));
  Button *b = (Button *)malloc(sizeof(Button));

  b->id = id;
  b->space = box;
  b->text = str;

  newList->val = b;
  newList->next = list;
  return newList;
}

Button *GetHoveredButton(L list, int mousex, int mousey) {
  Button *b;
  do {
    b = list->val;
    if (b != NULL && IsPointHit(&b->space, mousex, mousey)) {
      return b;
    }
    list = list->next;
  } while (list);

  return NULL;
}

#undef L