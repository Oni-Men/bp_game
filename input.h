#include <handy.h>

typedef struct
{
  int keyStates[256 * 256];
  int modKey;
  int mouse_down;
  int mouse_x;
  int mouse_y;
} Input;

void processInput(hgevent *e);
int isKeyPressed(int key);
int isMouseDown();
int getMouseX();
int getMouseY();
int getModKey();