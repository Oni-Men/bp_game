#include "input.h"

#include <stdbool.h>
#include <stdio.h>

Input input;

void initInput(Input *i) {
  i->modKey = 0;
  i->mouse_down = false;
  i->mouse_x = 0;
  i->mouse_y = 0;
}

void processInput(hgevent *e) {
  if (e == NULL) {
    return;
  }

  input.modKey = e->modkey;

  if (e->type == HG_MOUSE_MOVE) {
    input.mouse_x = e->x;
    input.mouse_y = e->y;
  }

  if (e->type == HG_MOUSE_DOWN) {
    input.mouse_down = true;
  }

  if (e->type == HG_MOUSE_UP) {
    input.mouse_down = false;
  }

  if (e->type == HG_KEY_DOWN) {
    input.keyStates[e->ch] = 1;
  }
  if (e->type == HG_KEY_UP) {
    input.keyStates[e->ch] = 0;
  }
}

int isKeyPressed(int key) {
  if (key < 0 || key >= MAX_KEY_CODE) {
    return false;
  }

  int state = input.keyStates[key];
  if (state == 0) {
    return false;
  }

  return state == 1;
}

int isKeyDown(int key) {
  if (key < 0 || key >= MAX_KEY_CODE) {
    return false;
  }
  return input.keyStates[key];
}

int isMouseDown() { return input.mouse_down; }

int getMouseX() { return input.mouse_x; }

int getMouseY() { return input.mouse_y; }