/**
 * @file main.c
 * @author Yamamoto Ryotaro (eurekaxe@gmail.com)
 * @brief
 * マッチョを倒してプロテインをゲットするゲームです。
 * 起動時にかなりの確率でセグフォが発生する、プレイ中にセグフォが発生する不具合があります。
 * 操作方法：
 *  W：上に加速
 *  A：左移動
 *  D：右移動
 *  S：下に加速
 *  スペースキー：マウスカーソルの方向に弾幕を発射
 * 工夫した点
 *  ・ダブルバッファリング
 *  ・FPS調整
 *  ・カメラワーク
 * @version 0.1
 * @date 2022-07-13
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "main.h"

#include <handy.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "words.h"

extern Input input;

// #define TEST
#ifdef TEST
int main(void) {}
#else

int main() {
  doubleLayer layers;
  int window, layer;
  int now, elapsed;

  window = HgOpen(WINDOW_WIDTH, WINDOW_HEIGHT);
  layers = HgWAddDoubleLayer(window);

  HgWSetTitle(window, WINDOW_TITLE);
  HgSetEventMask(HG_KEY_EVENT_MASK | HG_MOUSE_EVENT_MASK);

  Game game;
  initAll(&game);
  initTitle(&game);

  game.player.space.width = 96;
  game.player.space.height = 96;

  hgevent *e;
  while (!game.exit) {
    // 現在時刻(ミリ秒）
    now = milliSecAtNow();

    // 直前にフレームが更新されてから経過した時間（ミリ秒）
    elapsed = now - game.timeAtLastFrame;

    e = HgEventNonBlocking();
    processInput(e);
    if (e != NULL && e->type == HG_KEY_DOWN) {
      switch (e->ch) {
        case PAUSE_TOGGLE_KEY:
          game.pause = !game.pause;
          break;
        case DEBUG_TOGGLE_KEY:
          game.showDebug = !game.showDebug;
        default:
          break;
      }
    }

    // FPSがちょうどになるようにフレーム更新間隔を調整
    if (elapsed > (1000 / FPS)) {
      srand((unsigned int)elapsed);

      //ダブルバッファリング有効のとき、バッファを切り替える
      if (game.useDoubleLayer) {
        layer = HgLSwitch(&layers);
      } else {
        layer = layers.display;
      }

      switch (game.state) {
        case TITLE:
          TickTitle(layer, &game);
          break;
        case PLAY:
          TickPlay(layer, &game);
          break;
        case RESULT:
          TickResult(layer, &game);
          break;
        case EXIT:
          TickExit(layer, &game);
          break;
        default:
          game.exit = true;
          break;
      }

      if (game.showDebug) {
        RenderDebugLog(layer, &game);
      }
      game.timeAtLastFrame = now;
      game.fps = 1000.0 / elapsed;
    }
    HgSleep(1.0 / 1000.0);
  }

  HgClose();
  return 0;
}

#endif

void initAll(Game *game) {
  initInput(&input);
  initGame(game);
  LoadWords();
  LoadAssets(game);
  initButtons(game, WINDOW_WIDTH, WINDOW_HEIGHT);
}

void LoadAssets(Game *game) {
  loadTexture(game->texMap, TEX_BG, "./images/background/01.jpg");
  loadTexture(game->texMap, TEX_PLAYER, "./images/kuya.png");
  loadTexture(game->texMap, TEX_ENEMY, "./images/enemy.png");
  loadTexture(game->texMap, TEX_ITEM, "./images/protein.png");
  loadTexture(game->texMap, TEX_HEALTH, "./images/niku_manga.png");
  loadTexture(game->texMap, TEX_BULLET, "./images/arrow.png");
}

int RenderTextCenter(int layer, int x, int y, const char *str) {
  double tw, th;
  HgWTextSize(layer, &tw, &th, str);
  HgWText(layer, x - tw / 2, y - th / 2, str);
  return 0;
}

int RenderTextRight(int layer, int x, int y, const char *str) {
  double tw, th;
  HgWTextSize(layer, &tw, &th, str);
  HgWText(layer, x - tw, y, str);
  return 0;
}

void RenderDebugLog(int layer, Game *game) {
  HgWSetColor(layer, HG_BLACK);
  HgWSetFont(layer, HG_C, 16);

  int px, py;
  int x = 4, y = WINDOW_HEIGHT - 20;

  px = game->player.space.posx;
  py = game->player.space.posy;

  HgWText(layer, x, y, "fps: %.2f", game->fps);
  y -= 20;
  HgWText(layer, x, y, "mx: %d, my: %d", getMouseX(), getMouseY());
  y -= 20;
  HgWText(layer, x, y, "mouse: %s", (isMouseDown() ? "down" : "up"));
  y -= 20;
  HgWText(layer, x, y, "px: %d, py: %d", px, py);
  y -= 20;
  HgWText(layer, x, y, "camera: %.1f, %.1f", game->cameraPos.x,
          game->cameraPos.y);
  y -= 20;
  HgWText(layer, x, y, "bullets: %d", game->bulletsCount);
  y -= 20;
  HgWText(layer, x, y, "enemies: %d", game->enemyCount);
  y -= 20;
}

/**
 * @brief Render TickTitle scene
 *
 * @param layer
 * @param game
 */
void TickTitle(int layer, Game *game) {
  // Clear this window
  HgWSetFillColor(layer, HG_WHITE);
  HgWBoxFill(layer, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

  RenderTitleBackground(layer, game);
  RenderTitleLogo(layer, game);

  RenderAllButton(layer, getButtonList(game), &RenderTitleButton);

  int id = handleButtons(game);
  switch (id) {
    case 0:
      SetGameState(game, PLAY);
      break;
    case 1:
      SetGameState(game, EXIT);
    default:
      break;
  }
}

void RenderTitleBackground(int layer, Game *game) {
  int tex = getTextureId(game->texMap, TEX_BG);
  double img_w, img_h, aspect;
  HgImageSize(tex, &img_w, &img_h);
  aspect = (double)WINDOW_HEIGHT / (double)WINDOW_WIDTH;
  double vy = fmin(10, (img_h - WINDOW_HEIGHT - game->cameraPos.y) / 10);
  game->cameraPos.y += vy;
  double sw = img_w;
  double sh = img_w * aspect;
  double sx = game->cameraPos.x;
  double sy = game->cameraPos.y;

  HgWImageDrawRect(layer, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, tex, sx, sy, sw,
                   sh);
}

void RenderTitleLogo(int layer, Game *game) {
  int tex = getTextureId(game->texMap, TEX_BG);
  double img_w, img_h;
  HgImageSize(tex, &img_w, &img_h);
  int camera_y = game->cameraPos.y;
  int border = img_h - WINDOW_HEIGHT * 2.5;
  if (camera_y < border) {
    return;
  }

  double posx = WINDOW_WIDTH / 2;
  double posy = WINDOW_HEIGHT * (2.0 / 3.0);
  double alpha = (double)(camera_y - border) / (WINDOW_HEIGHT * 1.5);
  int logoColor = HgRGBA(1.0, 1.0, 1.0, alpha);
  HgWSetColor(layer, logoColor);
  HgWSetFontByName(layer, NormalFont, 36.0);
  RenderTextCenter(layer, posx, posy, WINDOW_TITLE);
}

void RenderTitleButton(int layer, Button *button) {
  if (button == NULL) {
    return;
  }

  double x = button->space.posx;
  double y = button->space.posy;
  double w = button->space.width;
  double h = button->space.height;

  double xp[] = {x - 10, x + w, x + w + 10, x};
  double yp[] = {y, y, y + h, y + h};

  double alpha = 0.25;
  int bg = HgRGBA(1.0, 1.0, 1.0, alpha);

  HgWSetWidth(layer, 2.5);
  HgWSetColor(layer, HG_WHITE);
  HgWSetFillColor(layer, bg);
  HgWPolygonFill(layer, 4, &xp[0], &yp[0], 1);

  HgWSetFontByName(layer, NormalFont, 24.0);
  HgWSetColor(layer, HG_WHITE);

  double tw, th;
  HgWTextSize(layer, &tw, &th, button->text);
  HgWText(layer, x + (w - tw) / 2, y + (h - th) / 2 + 4, button->text);
}

/**
 * @brief Render playing scene
 *
 * @param layer
 * @param game
 */
void TickPlay(int layer, Game *game) {
  HgWSetFillColor(layer, 0);
  HgWBoxFill(layer, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

  Space2d playerBox = game->player.space;
  double pCenterX = playerBox.posx + playerBox.width / 2;
  double pCenterY = playerBox.posy + playerBox.height / 2;

  // Camera Following
  if (!game->pause) {
    game->cameraPos.x += (pCenterX - game->cameraPos.x) / 5;
    game->cameraPos.y += (pCenterY - game->cameraPos.y) / 5;
  }

  RenderPlayBackground(layer, game);
  RenderPlayer(layer, game);
  RenderEnemies(layer, game);
  RenderBullets(layer, game);

  RenderStatus(layer, game);

  if (game->pause) {
    HandlePause(game);
    RenderPause(layer, game);
  } else {
    UpdateGame(game);
  }
}

void RenderPlayBackground(int layer, Game *game) {
  int tex = getTextureId(game->texMap, TEX_BG);
  double img_w, img_h, aspect;
  HgImageSize(tex, &img_w, &img_h);
  aspect = (double)WINDOW_HEIGHT / (double)WINDOW_WIDTH;
  double sw = img_w;
  double sh = img_w * aspect;
  double sx = 0;
  double sy = ((game->cameraPos.y) / (FIELD_HEIGHT)) * (img_h - WINDOW_HEIGHT);
  HgWImageDrawRect(layer, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, tex, sx, sy, sw,
                   sh);
}

void HandlePause(Game *game) {
  int id = handleButtons(game);
  switch (id) {
    case 0:
      game->pause = false;
      break;
    case 1:
      SetGameState(game, RESULT);
    default:
      break;
  }
}

void RenderPlayer(int layer, Game *game) {
  int ptex = getTextureId(game->texMap, TEX_PLAYER);
  Entity *player = &game->player;

  double img_w, img_h;
  HgImageSize(ptex, &img_w, &img_h);
  Space2d space = player->space;
  double x = space.posx - game->cameraPos.x + WINDOW_WIDTH / 2;
  double y = space.posy - game->cameraPos.y + WINDOW_HEIGHT / 2;
  double w = space.width;
  double h = space.height;

  HgWImagePut(layer, x + w / 2, y + h / 2, ptex, 0.5, player->rotation);

  if (game->showDebug) {
    HgWSetWidth(layer, 1);
    HgWSetColor(layer, HG_GREEN);
    HgWBox(layer, x, y, space.width, space.height);
  }
}

void RenderPause(int layer, Game *game) {
  HgWSetFillColor(layer, HgRGBA(0, 0, 0, 0.75));
  HgWBoxFill(layer, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

  HgWSetFontByName(layer, NormalFont, 36.0);
  HgWSetColor(layer, HG_WHITE);
  RenderTextCenter(layer, WINDOW_WIDTH / 2, WINDOW_HEIGHT - 60, "ポーズ中...");

  RenderAllButton(layer, getButtonList(game), &RenderTitleButton);
}

void RenderStatus(int layer, Game *game) {
  int tex;
  int x = WINDOW_WIDTH - 48;
  int y = WINDOW_HEIGHT - 40;

  Status *status = &game->player.status;

  HgWSetFontByName(layer, FuturaFont, 24);

  //体力ゲージを描画する
  tex = getTextureId(game->texMap, TEX_HEALTH);
  for (int i = 0; i < status->health; i++) {
    HgWImageDrawRect(layer, x - 24 * i, y, 32, 32, tex, 0, 0, 256, 256);
  }

  y -= 32;

  //スコアゲージを描画する
  tex = getTextureId(game->texMap, TEX_ITEM);
  HgWImageDrawRect(layer, x - 100, y, 32, 32, tex, 0, 0, 256, 256);
  HgWSetColor(layer, HG_WHITE);
  HgWText(layer, x - 40, y - 4, "%d", status->score);
}

void RenderBullets(int layer, Game *game) {
  Entity *e;
  Space2d space;
  double x, y;

  int tex = getTextureId(game->texMap, TEX_BULLET);

  HgWSetFontByName(layer, NormalFont, 24.0);
  HgWSetColor(layer, HG_WHITE);
  for (int i = 0; i < MAX_BULLETS; i++) {
    e = game->bullets[i];
    if (e == NULL) {
      continue;
    }

    if (!IsAlive(&e->status)) {
      continue;
    }

    space = e->space;
    x = space.posx - game->cameraPos.x + WINDOW_WIDTH / 2;
    y = space.posy - game->cameraPos.y + WINDOW_HEIGHT / 2;
    HgWImagePut(layer, x + space.width / 2, y + space.height / 2, tex, 0.1,
                e->rotation);

    if (game->showDebug) {
      HgWSetWidth(layer, 1);
      HgWSetColor(layer, HG_BLUE);
      HgWBox(layer, x, y, BULLET_SIZE, BULLET_SIZE);
      HgWSetColor(layer, HG_WHITE);
    }
  }
}

void RenderEnemies(int layer, Game *game) {
  Entity *e;
  Space2d space;
  double x, y, scale;

  int tex = getTextureId(game->texMap, TEX_ENEMY);

  for (int i = 0; i < MAX_ENEMIES; i++) {
    e = game->enemies[i];
    if (e == NULL) {
      continue;
    }

    space = e->space;
    x = space.posx - game->cameraPos.x + WINDOW_WIDTH / 2;
    y = space.posy - game->cameraPos.y + WINDOW_HEIGHT / 2;

    scale = 0.5;
    if (!IsAlive(&e->status)) {
      scale -= fmax(0.5, fmin(0.0, e->status.tickDead / 200.0));
    }
    if (scale > 0) {
      HgWImagePut(layer, x + space.width / 2, y + space.height / 2, tex, scale,
                  e->rotation);
    }

    if (game->showDebug) {
      HgWSetWidth(layer, 1);
      HgWSetColor(layer, HG_RED);
      HgWBox(layer, x, y, space.width, space.height);
    }
  }
}

void TickResult(int layer, Game *game) {
  HgWSetFillColor(layer, HG_BLACK);
  HgWBoxFill(layer, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

  HgWSetFontByName(layer, NormalFont, 36.0);
  HgWSetColor(layer, HG_WHITE);
  RenderTextCenter(layer, WINDOW_WIDTH / 2, WINDOW_HEIGHT - 60, "結果");
  HgWSetFontByName(layer, NormalFont, 24.0);
  RenderTextCenter(layer, WINDOW_WIDTH / 2, WINDOW_HEIGHT - 200,
                   "ゲットしたプロテイン");

  HgWSetFontByName(layer, FuturaFont, 48.0);
  int score = game->player.status.score;
  char scoreText[10];
  double scoreTextWidth, scoreTextHeight;
  HgWSetColor(layer, HG_YELLOW);
  HgWTextSize(layer, &scoreTextWidth, &scoreTextHeight, "%d", score);
  HgWText(layer, (WINDOW_WIDTH - scoreTextWidth) / 2,
          (WINDOW_HEIGHT - scoreTextHeight) / 2 - 50, "%d", score);

  RenderAllButton(layer, getButtonList(game), &RenderTitleButton);

  int id = handleButtons(game);
  switch (id) {
    case 0:
      SetGameState(game, TITLE);
      break;
    default:
      break;
  }
}

void TickExit(int layer, Game *game) {
  ExitSceneMeta *meta = &game->exitSceneMeta;
  double a = meta->frames > 100 ? 1.0 : 0.05;
  int c = HgGrayA(0.0, a);
  HgWSetFillColor(layer, c);
  HgWBoxFill(layer, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

  if (meta->frames++ > 120) {
    game->exit = true;
  }
}