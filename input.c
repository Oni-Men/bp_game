#include <stdio.h>
#include <stdbool.h>
#include "input.h"

Input input;

void processInput(hgevent *e)
{
  if (e == NULL)
  {
    return;
  }

  input.modKey = e->modkey;

  if (e->type == HG_MOUSE_MOVE)
  {
    input.mouse_x = e->x;
    input.mouse_y = e->y;
  }

  if (e->type == HG_MOUSE_DOWN)
  {
    input.mouse_down = true;
  }

  if (e->type == HG_MOUSE_UP)
  {
    input.mouse_down = false;
  }

  if (e->type == HG_KEY_DOWN)
  {
    input.keyStates[e->ch] = true;
  }
  if (e->type == HG_KEY_UP)
  {
    input.keyStates[e->ch] = false;
  }
}

int isKeyPressed(int key)
{
  if (key < 0 || key >= 255)
  {
    return false;
  }
  return input.keyStates[key];
}

int isMouseDown()
{
  return input.mouse_down;
}

int getMouseX()
{
  return input.mouse_x;
}

int getMouseY()
{
  return input.mouse_y;
}