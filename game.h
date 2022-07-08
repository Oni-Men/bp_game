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

#define MAX_BULLETS (100)
#define BULLET_SIZE (16)

#define MAX_ENEMIES (100)

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
  int frames;
} ExitSceneMeta;

typedef struct {
  int id;
  Entity *e;
  char *s;
} Bullet;

typedef struct {
  /** @brief Debugを表示するか */
  int showDebug;

  int useDoubleLayer;

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

  int bulletsCount;
  Bullet *bullets[MAX_BULLETS];

  int enemyCount;
  Entity enemies[MAX_ENEMIES];

  ButtonList buttons[3];

  ExitSceneMeta exitSceneMeta;
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

/**
 * @brief プレイヤーのアクションを実行する
 *
 * @param game
 */
void PlayAct(Game *game);

void SetGameState(Game *game, int state);

void initButtons(Game *game, double width, double height);

/**
 * @brief タイトル画面に切り替わった時に実行される
 *
 * @param game
 */
void initTitle(Game *game);

/**
 * @brief プレイ画面に切り替わった時に実行される
 *
 * @param game
 */
void initPlay(Game *game);

/**
 * @brief リザルト画面に切り替わった時に実行される
 *
 * @param game
 */
void initResult(Game *game);

/**
 * @brief 終了画面に切り替わった時に実行される
 *
 * @param game
 */
void initExit(Game *game);

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

/**
 * @brief 弾丸オブジェクトを追加する
 *
 * @param game
 * @param bullet
 */
void AddBullet(Game *game, Bullet *bullet);

/**
 * @brief 弾丸オブジェクトを削除し、メモリを解放する
 *
 * @param game
 * @param b
 */
void RemoveBullet(Game *game, Bullet *b);

/**
 * @brief すべての弾丸オブジェクトについて、処理を行う
 *
 * @param game
 */
void UpdateBullets(Game *game);

#endif