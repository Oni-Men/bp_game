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

#define WINDOW_WIDTH (1080)
#define WINDOW_HEIGHT (720)
#define FPS (60.0)

/**
 * @brief 現在のミリ秒を取得する
 *
 * @return int
 */
int currentTimeMillis();

/**
 * @brief 画像や音声などをファイルから読み込む
 *
 * @param game
 */
void loadResources(Game *game);

/**
 * @brief titleシーンのティック処理
 *
 * @param layer
 * @param game
 */
void title(int layer, Game *game);

/**
 * @brief playシーンのティック処理
 *
 * @param layer
 * @param game
 */
void play(int layer, Game *game);

/**
 * @brief resultシーンのティック処理
 *
 * @param layer
 * @param game
 */
void result(int layer, Game *game);

/**
 * @brief デバッグ情報を描画
 *
 * @param layer
 * @param game
 */
void renderDebugLog(int layer, Game *game);

/**
 * @brief プレイヤーのステータス情報を描画
 *
 * @param layer
 * @param game
 */
void renderStatus(int layer, Game *game);

/**
 * @brief ポーズ画面を描画
 *
 * @param layer
 * @param game
 */
void renderPauseScreen(int layer, Game *game);

/**
 * @brief プレイヤーを描画
 *
 * @param layer
 * @param game
 */
void renderPlayer(int layer, Game *game);

/**
 * @brief 背景を描画
 *
 * @param layer
 * @param game
 */
void renderBackground(int layer, Game *game);

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
 * @brief 文字列を、中央寄せで描画
 *
 * @param layer
 * @param c
 * @param y
 * @param str
 * @return int
 */
int renderTextCenter(int layer, int c, int y, const char *str);

/**
 * @brief 文字列を、右寄せで描画
 *
 * @param layer
 * @param c
 * @param y
 * @param str
 * @return int
 */
int renderTextRight(int layer, int c, int y, const char *str);
#endif