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
int main(void) {
  LoadWords();
  char *word;
  word = GetRandomWord();

  printf("%s\n", word);
}
#else

int main() {
  doubleLayer layers, bulletLayers;
  int window, layer, bulletLayer;
  int now, elapsed;

  window = HgOpen(WINDOW_WIDTH, WINDOW_HEIGHT);
  layers = HgWAddDoubleLayer(window);
  bulletLayers = HgWAddDoubleLayer(window);

  HgWSetTitle(window, WINDOW_TITLE);
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
        bulletLayer = HgLSwitch(&bulletLayers);
      } else {
        layer = layers.display;
        bulletLayer = bulletLayers.display;
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
  LoadWords();
  LoadAssets(game);
  initButtons(game, WINDOW_WIDTH, WINDOW_HEIGHT);
}

void LoadAssets(Game *game) {
  putTexture(game->texMap, loadTexture("./images/background/01.jpg"));
  putTexture(game->texMap, loadTexture("./images/kuya.png"));
  putTexture(game->texMap, loadTexture("./images/arrow.png"));
  putTexture(game->texMap, loadTexture("./images/shield.png"));
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
  HgWText(layer, x, y, "bullets: %d", game->bulletsCount);
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
  HgWSetFontByName(layer, KaiseiFont, 36.0);
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
  HgWPolygonFill(layer, 4, &xp, &yp, 1);

  HgWSetFontByName(layer, KaiseiFont, 24.0);
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
    game->cameraPos.x += (playerBox.posx - game->cameraPos.x) / 5;
    game->cameraPos.y += (playerBox.posy - game->cameraPos.y) / 5;
  }

  RenderPlayBackground(layer, game);
  // RenderMap(layer, game->map, &game->cameraPos);
  RenderPlayer(layer, game);
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
  int tex = getTextureId(game->texMap, "./images/background/01.jpg");
  double img_w, img_h, aspect;
  HgImageSize(tex, &img_w, &img_h);
  aspect = (double)WINDOW_HEIGHT / (double)WINDOW_WIDTH;
  double sw = img_w;
  double sh = img_w * aspect;
  double sx = 0;
  double sy = ((game->cameraPos.y) / (25000)) * (img_h - WINDOW_HEIGHT);
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
  int ptex = getTextureId(game->texMap, "./images/kuya.png");
  Entity *player = &game->player;

  double img_w, img_h;
  HgImageSize(ptex, &img_w, &img_h);
  Space2d space = player->space;
  double x = space.posx - game->cameraPos.x + WINDOW_WIDTH / 2;
  double y = space.posy - game->cameraPos.y + WINDOW_HEIGHT / 2;
  double w = space.width;
  double h = space.height;

  // HgWImageDrawRect(layer, x, y, w, h, ptex, 0, 0, img_w, img_h);

  HgWBox(layer, x, y, space.width, space.height);
}

void RenderPause(int layer, Game *game) {
  HgWSetFillColor(layer, HgRGBA(0, 0, 0, 0.75));
  HgWBoxFill(layer, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

  HgWSetFontByName(layer, KaiseiFont, 36.0);
  HgWSetColor(layer, HG_WHITE);
  RenderTextCenter(layer, WINDOW_WIDTH / 2, WINDOW_HEIGHT - 60, "ポーズ中...");

  RenderAllButton(layer, getButtonList(game), &RenderTitleButton);
}

void RenderStatus(int layer, Game *game) {
  int x = WINDOW_WIDTH - 100;
  int y = WINDOW_HEIGHT - 40;

  char buf[30];
  Status *status = &game->player.status;
  FormatScore(status, buf);

  HgWSetFontByName(layer, FuturaFont, 24);

  // Render player's score
  int tex = getTextureId(game->texMap, "./images/shield.png");
  HgWImageDrawRect(layer, x, y, 24, 24, tex, 0, 0, 128, 128);
  HgWSetColor(layer, HG_WHITE);
  HgWText(layer, x + 24, y - 4, "%s", buf);

  y -= 32;

  // Render player's remaining ammo
  tex = getTextureId(game->texMap, "./images/arrow.png");
  HgWImageDrawRect(layer, x, y, 24, 24, tex, 0, 0, 128, 128);
  HgWText(layer, x + 24, y - 4, "%d", status->ammoRemaining);
}

void RenderBullets(int layer, Game *game) {
  Bullet *b;
  Entity *e;
  Space2d space;
  double x, y;
  int tex = getTextureId(game->texMap, "./images/arrow.png");

  HgWSetFontByName(layer, KaiseiFont, 24.0);
  HgWSetColor(layer, HG_WHITE);
  for (int i = 0; i < MAX_BULLETS; i++) {
    b = game->bullets[i];
    if (b == NULL) {
      continue;
    }
    e = b->e;
    space = e->space;
    x = space.posx - game->cameraPos.x + WINDOW_WIDTH / 2;
    y = space.posy - game->cameraPos.y + WINDOW_HEIGHT / 2;
    // HgWImagePut(layer, x, y, tex, 0.2, e->rotation);
    HgWText(layer, x, y, "%s", b->s);
  }
}

void TickResult(int layer, Game *game) {
  HgWSetFillColor(layer, HG_WHITE);
  HgWBoxFill(layer, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

  HgWSetFontByName(layer, KaiseiFont, 36.0);
  HgWSetColor(layer, HG_BLACK);
  HgWText(layer, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, "結果");
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