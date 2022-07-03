#ifndef GAME_H
#define GAME_H

#include <handy.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
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

typedef enum GameState {
  TITLE,
  PLAY,
  RESULT,
  EXIT,
} GameState;

typedef struct {
  int loop;
} TitleMeta;

typedef struct {
  /** @brief Debugを表示するか */
  int showDebug;

  /** @brief ゲームの状態 */
  int state;
  int pause;
  int exit;
  double fps;
  time_t timeAtLastFrame;

  Vec2d cameraPos;
  Map *map;
  TextureMap *texMap;
  Entity player;

  ButtonList buttons[3];
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
void UpdateGame(Game *game);

void SetGameState(Game *game, int state);

void initButtons(Game *game, double width, double height);

void initTitle(Game *game);

void initPlay(Game *game);

/**
 * @brief 与えられたゲーム状態に対するボタンリストを取得する
 *
 * @param game
 * @param state
 * @return ButtonList*
 */
ButtonList getButtonList(Game *game);

/**
 * @brief
 * 押されているボタンのIDを調べる。押されているボタンがないときは-1を返す。
 *
 * @param game
 * @return int ボタンのID
 */
int handleButtons(Game *game);

/**
 * @brief 与えられた空間とタイルが衝突しているかを判定
 *
 * @param space
 * @param tile
 * @return true
 * @return false
 */
bool checkTileHit(Space2d *space, Tile *tile);

/**
 * @brief マップ内で、スペースが衝突しているタイルを取得
 *
 * @param space
 * @param map
 * @return Tile*
 */
Tile *getHitTile(Space2d *space, Map *map);

#endif