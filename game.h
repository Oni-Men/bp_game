#include <sys/time.h>
#include <time.h>

#include "button.h"
#include "entity.h"
#include "input.h"
#include "map.h"
#include "space.h"
#include "status.h"
#include "texture.h"

#define STATE_TITLE 0
#define STATE_PLAY 1
#define STATE_RESULT 2

typedef struct {
  /** @brief Debugを表示するか */
  int showDebug;

  /** @brief ゲームの状態 */
  int state;
  int pause;
  int exit;
  double fps;
  time_t timeAtLastFrame;

  ButtonList buttonsList[3];
  Map *map;
  TextureMap *texMap;

  Entity player;

  Vec2d cameraPos;

} Game;

/**
 * @brief ゲームオブジェクトの初期化をする
 *
 * @param game
 */
void initGame(Game *game);

/**
 * @brief ゲームの更新処理
 *
 * @param game
 */
void updateGame(Game *game);

void updateButtons(Game *game, double width, double height);

/**
 * @brief 与えられたゲーム状態に対するボタンリストを取得する
 *
 * @param game
 * @param state
 * @return ButtonList*
 */
ButtonList *getButtonList(Game *game);
