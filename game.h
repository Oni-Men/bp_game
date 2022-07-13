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
#include "space.h"
#include "status.h"
#include "texture.h"

#define FIELD_HEIGHT (25000)

#define MAX_BULLETS (100)
#define BULLET_SIZE (16)

#define MAX_ENEMIES (20)

typedef enum GameState {
  TITLE,
  PLAY,
  RESULT,
  EXIT,
} GameState;

typedef struct {
  int frames;
} ExitSceneMeta;

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
  TextureMap *texMap;
  Entity player;

  int bulletsCount;
  Entity *bullets[MAX_BULLETS];

  int enemyCount;
  int lastEnemySpawned;
  Entity *enemies[MAX_ENEMIES];

  ButtonList buttons[3];

  ExitSceneMeta exitSceneMeta;
} Game;

double range_randf(double min, double max);

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
 * @brief 弾丸オブジェクトを追加する
 *
 * @param game
 * @param bullet
 */
void AddBullet(Game *game, Entity *bullet);

/**
 * @brief 弾丸オブジェクトを削除し、メモリを解放する
 *
 * @param game
 * @param b
 */
void RemoveBullet(Game *game, Entity *b);

/**
 * @brief すべての弾丸オブジェクトについて、処理を行う
 *
 * @param game
 */
void UpdateBullets(Game *game);

/**
 * @brief 敵を追加する
 *
 * @param game
 * @param e
 */
void AddEnemy(Game *game, Entity *e);

/**
 * @brief 敵を削除する
 *
 * @param game
 * @param e
 */
void RemoveEnemy(Game *game, Entity *e);

/**
 * @brief すべての敵について、更新処理を行う
 *
 * @param game
 */
void UpdateEnemies(Game *game);

#endif