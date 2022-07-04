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

extern Input input;

// #define TEST
#ifdef TEST
int main(void) {
  Space2d a = {10, 10, 10, 10};
  Space2d b = {0, 0, 15, 15};
  Space2d c = {25, 25, 10, 10};

  printf("A:B = %d\n", IsBoxHit(&a, &b));
  printf("B:C = %d\n", IsBoxHit(&b, &c));
  printf("C:A = %d\n", IsBoxHit(&c, &a));
}
#else

int main() {
  doubleLayer layers;
  int window, layer;
  int now, elapsed;

  window = HgOpen(WINDOW_WIDTH, WINDOW_HEIGHT);
  layers = HgWAddDoubleLayer(window);

  HgSetEventMask(HG_KEY_EVENT_MASK | HG_MOUSE_EVENT_MASK);

  Game game;
  initAll(&game);

  // TODO
  // プレイヤーの画像サイズからプレイヤーのサイズを設定しているが、これを変える
  double imgW, imgH;
  HgImageSize(getTextureId(game.texMap, "./images/kuya.png"), &imgW, &imgH);
  game.player.space.width = imgW / 4;
  game.player.space.height = imgH / 4;

  hgevent *e;
  while (!game.exit) {
    now = milliSecAtNow();
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

    if (elapsed > (1000 / FPS)) {
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

  unloadMap(game.map);

  HgClose();
  return 0;
}

#endif

void initAll(Game *game) {
  initInput(&input);
  initGame(game);
  LoadAssets(game);
  initButtons(game, WINDOW_WIDTH, WINDOW_HEIGHT);
}

void LoadAssets(Game *game) {
  putTexture(game->texMap, loadTexture("./images/background/01.jpg"));
  putTexture(game->texMap, loadTexture("./images/kuya.png"));
  putTexture(game->texMap, loadTexture("./images/coin.png"));
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
  Tile *tile = game->player.hitTile;
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
  HgWText(layer, x, y, "tx: %d, ty: %d", px / TILE_SIZE, py / TILE_SIZE);
  y -= 20;
  if (tile != NULL) {
    HgWText(layer, x, y, "tile: %d", tile->type);
  }
  y -= 20;
  HgWText(layer, x, y, "camera: %.1f, %.1f", game->cameraPos.x,
          game->cameraPos.y);
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
  int tex = getTextureId(game->texMap, "./images/background/01.jpg");
  double img_w, img_h, aspect;
  HgImageSize(tex, &img_w, &img_h);
  double vy = fmin(10, (img_h - WINDOW_HEIGHT - game->cameraPos.y) / 10);
  game->cameraPos.y += vy;
  aspect = (double)WINDOW_HEIGHT / (double)WINDOW_WIDTH;
  double sw = img_w;
  double sh = img_w * aspect;
  double sx = game->cameraPos.x;
  double sy = game->cameraPos.y;

  HgWImageDrawRect(layer, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, tex, sx, sy, sw,
                   sh);
}

void RenderTitleLogo(int layer, Game *game) {
  int tex = getTextureId(game->texMap, "./images/background/01.jpg");
  double img_w, img_h, aspect;
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
  HgWSetFont(layer, HG_M, 36);
  RenderTextCenter(layer, posx, posy, "自由落下");
}

void RenderTitleButton(int layer, Button *button) {
  if (button == NULL) {
    return;
  }

  int x = button->space.posx;
  int y = button->space.posy;
  int w = button->space.width;
  int h = button->space.height;

  double alpha = 0.25;
  int bg = HgRGBA(1.0, 1.0, 1.0, alpha);
  HgWSetFillColor(layer, bg);
  HgWBoxFill(layer, x, y, w, h, 0);

  HgWSetFont(layer, HG_M, 24.0);
  HgWSetColor(layer, HG_WHITE);

  double tw, th;
  HgWTextSize(layer, &tw, &th, button->text);
  HgWText(layer, x + (w - tw) / 2, y + (h - th) / 2, button->text);
}

/**
 * @brief Render playing scene
 *
 * @param layer
 * @param game
 */
void TickPlay(int layer, Game *game) {
  HgWSetFillColor(layer, HG_WHITE);
  HgWBoxFill(layer, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

  Space2d playerBox = game->player.space;
  double pCenterX = playerBox.posx + playerBox.width / 2;
  double pCenterY = playerBox.posy + playerBox.height / 2;

  // Camera Following
  if (!game->pause) {
    game->cameraPos.x += (playerBox.posx - game->cameraPos.x) / 5;
    game->cameraPos.y += (playerBox.posy - game->cameraPos.y) / 5;
  }

  RenderPlayBackground(layer, game);
  // RenderMap(layer, game->map, &game->cameraPos);
  RenderPlayer(layer, game);
  RenderStatus(layer, game);

  if (game->pause) {
    RenderPause(layer, game);
  } else {
    UpdateGame(game);
  }
}

void RenderPlayBackground(int layer, Game *game) {
  int tex = getTextureId(game->texMap, "./images/background/01.jpg");
  double img_w, img_h, aspect;
  HgImageSize(tex, &img_w, &img_h);
  aspect = (double)WINDOW_HEIGHT / (double)WINDOW_WIDTH;
  double sw = img_w;
  double sh = img_w * aspect;
  double sx = 0;
  double sy = (game->cameraPos.y / 50000) * img_h;
  HgWImageDrawRect(layer, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, tex, sx, sy, sw,
                   sh);
}

void RenderPlayer(int layer, Game *game) {
  int ptex = getTextureId(game->texMap, "./images/kuya.png");
  Entity *player = &game->player;

  double img_w, img_h;
  HgImageSize(ptex, &img_w, &img_h);
  Space2d space = player->space;
  double x = space.posx - game->cameraPos.x + WINDOW_WIDTH / 2;
  double y = space.posy - game->cameraPos.y + WINDOW_HEIGHT / 2;
  double w = space.width;
  double h = space.height;

  HgWImageDrawRect(layer, x, y, w, h, ptex, 0, 0, img_w, img_h);

  HgWBox(layer, x, y, space.width, space.height);
}

void RenderPause(int layer, Game *game) {
  HgWSetFillColor(layer, HgRGBA(0, 0, 0, 0.75));
  HgWBoxFill(layer, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

  HgWSetColor(layer, HG_WHITE);
  HgWSetFont(layer, HG_GB, 36);
  RenderTextCenter(layer, WINDOW_WIDTH / 2, WINDOW_HEIGHT - 100, "Pause");
}

void RenderStatus(int layer, Game *game) {
  int coinTex = getTextureId(game->texMap, "./images/coin.png");
  int x = WINDOW_WIDTH - 40;
  int y = WINDOW_HEIGHT - 40;

  HgWImagePut(layer, x - 100, y, coinTex, 0.1, 0);

  Status *status = &game->player.status;
  formatMoney(status);

  HgWSetFont(layer, HG_GB, 24);
  HgWSetColor(layer, HG_YELLOW);
  HgWText(layer, x - 80, y - 10, "x %s", status->moneyText);
}

void TickResult(int layer, Game *game) {
  HgWSetFillColor(layer, HG_WHITE);
  HgWBoxFill(layer, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
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