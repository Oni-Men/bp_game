#ifndef INPUT_H
#define INPUT_H
#include <handy.h>

#define MAX_KEY_CODE (65536)

typedef struct {
  int keyStates[MAX_KEY_CODE];
  int modKey;
  int mouse_down;
  int mouse_x;
  int mouse_y;
} Input;

void initInput(Input *i);

/**
 * @brief HgEventに対しイベントの種類ごとに適切な処理を行う。
 *
 * @param e
 */
void processInput(hgevent *e);

/**
 * @brief キーが押されているか確認する。長押しのときは、一度だけtrueを返す。
 *
 * @param key
 * @return int
 */
int isKeyPressed(int key);

/**
 * @brief キーが押されているか確認する。長押しのときは常にtrueを返す。
 *
 * @param key
 * @return int
 */
int isKeyDown(int key);

/**
 * @brief マウスが押されているか確認する
 *
 * @return int
 */
int isMouseDown();

/**
 * @brief マウスのX座標を確認する。
 *
 * @return int
 */
int getMouseX();

/**
 * @brief マウスのY座標を確認する
 *
 * @return int
 */
int getMouseY();
int getModKey();

#endif