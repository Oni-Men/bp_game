#ifndef MAIN_H
#define MAIN_H

#include <stdbool.h>

#include "button.h"
#include "entity.h"
#include "game.h"
#include "input.h"
#include "map.h"
#include "space.h"
#include "status.h"
#include "texture.h"
#include "timeutil.h"

#define WINDOW_WIDTH (1080)
#define WINDOW_HEIGHT (720)
#define FPS (60.0)

#define PAUSE_TOGGLE_KEY (0x1B)
#define DEBUG_TOGGLE_KEY (0x2F)

/**
 * @brief 初期化処理をまとめて行う
 *
 */
void initAll(Game *game);

/**
 * @brief 画像や音声などをファイルから読み込む
 *
 * @param game
 */
void LoadAssets(Game *game);

/**
 * @brief Titleシーンのティック処理
 *
 * @param layer
 * @param game
 */
void TickTitle(int layer, Game *game);

/**
 * @brief タイトル画面の背景を描画する
 *
 * @param layer
 * @param game
 */
void RenderTitleBackground(int layer, Game *game);

/**
 * @brief タイトルのロゴを描画する
 *
 * @param layer
 * @param game
 */
void RenderTitleLogo(int layer, Game *game);

/**
 *
 * @brief Playシーンのティック処理
 *
 * @param layer
 * @param game
 */
void TickPlay(int layer, Game *game);

/**
 * @brief Resultシーンのティック処理
 *
 * @param layer
 * @param game
 */
void TickResult(int layer, Game *game);

/**
 * @brief Exitシーンのティック処理
 *
 * @param layer
 * @param game
 */
void TickExit(int layer, Game *game);

/**
 * @brief デバッグ情報を描画
 *
 * @param layer
 * @param game
 */
void RenderDebugLog(int layer, Game *game);

/**
 * @brief プレイヤーのステータス情報を描画
 *
 * @param layer
 * @param game
 */
void RenderStatus(int layer, Game *game);

/**
 * @brief ポーズ画面を描画
 *
 * @param layer
 * @param game
 */
void RenderPause(int layer, Game *game);

/**
 * @brief プレイヤーを描画
 *
 * @param layer
 * @param game
 */
void RenderPlayer(int layer, Game *game);

/**
 * @brief 背景を描画
 *
 * @param layer
 * @param game
 */
void RenderPlayBackground(int layer, Game *game);

/**
 * @brief 文字列を、中央寄せで描画
 *
 * @param layer
 * @param c
 * @param y
 * @param str
 * @return int
 */
int RenderTextCenter(int layer, int c, int y, const char *str);

/**
 * @brief 文字列を、右寄せで描画
 *
 * @param layer
 * @param c
 * @param y
 * @param str
 * @return int
 */
int RenderTextRight(int layer, int c, int y, const char *str);

void RenderTitleButton(int layer, Button *button);

void RenderPauseButton(int layer, Button *button);

void RenderResultButton(int layer, Button *button);
#endif