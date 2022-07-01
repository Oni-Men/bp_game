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
  int escContinuous = 0;

  window = HgOpen(WINDOW_WIDTH, WINDOW_HEIGHT);
  layers = HgWAddDoubleLayer(window);

  HgSetEventMask(HG_KEY_EVENT_MASK | HG_MOUSE_EVENT_MASK);

  Game game;
  initGame(&game);
  loadResources(&game);

  double imgW, imgH;
  HgImageSize(getTextureId(game.texMap, "./images/kuya.png"), &imgW, &imgH);
  game.player.space.width = imgW / 4;
  game.player.space.height = imgH / 4;

  while (!game.exit) {
    now = currentTimeMillis();
    elapsed = now - game.timeAtLastFrame;
    processInput(HgEventNonBlocking());

    if (elapsed > (1000 / FPS)) {
      layer = HgLSwitch(&layers);

      if (isKeyPressed(0x1B)) {
        if (escContinuous == 0) {
          game.pause = !game.pause;
        }
        escContinuous++;
      } else {
        escContinuous = 0;
      }

      if (game.state == STATE_TITLE) {
        title(layer, &game);
      }
      if (game.state == STATE_PLAY) {
        play(layer, &game);
      }
      if (game.state == STATE_RESULT) {
        result(layer, &game);
      }

      renderDebugLog(layer, &game);
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

int currentTimeMillis() {
  struct timeval now;
  gettimeofday(&now, NULL);
  return (now.tv_sec * 1000) + (now.tv_usec / 1000);
}

void loadResources(Game *game) {
  putTexture(game->texMap, loadTexture("./images/background/01.jpg"));
  putTexture(game->texMap, loadTexture("./images/kuya.png"));
  putTexture(game->texMap, loadTexture("./images/coin.png"));
}

int renderTextCenter(int layer, int x, int y, const char *str) {
  double tw, th;
  HgWTextSize(layer, &tw, &th, str);
  HgWText(layer, x - tw / 2, y - th / 2, str);
  return 0;
}

int renderTextRight(int layer, int x, int y, const char *str) {
  double tw, th;
  HgWTextSize(layer, &tw, &th, str);
  HgWText(layer, x - tw, y, str);
  return 0;
}

void renderDebugLog(int layer, Game *game) {
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
}

/**
 * @brief Render title scene
 *
 * @param layer
 * @param game
 */
void title(int layer, Game *game) {
  // Clear the window
  HgWSetFillColor(layer, HG_WHITE);
  HgWBoxFill(layer, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

  // Draw title logo (as of now, it is empty)
  HgWSetColor(layer, HG_BLACK);
  HgWSetFont(layer, HG_M, 36);
  renderTextCenter(layer, WINDOW_WIDTH / 2, WINDOW_HEIGHT * (2.0 / 3.0), "");

  // Draw all buttons
  renderAllButton(layer, getButtonList(game));

  // Process button click
  if (!isMouseDown()) {
    return;
  }
  int hoveredId = getHoveredButton(game->buttonsList, getMouseX(), getMouseY());
  switch (hoveredId) {
    case 0:
      game->state = STATE_PLAY;
      break;
    case 1:
      game->exit = true;
    default:
      break;
  }
}

/**
 * @brief Render playing scene
 *
 * @param layer
 * @param game
 */
void play(int layer, Game *game) {
  HgWSetFillColor(layer, HG_WHITE);
  HgWBoxFill(layer, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

  Space2d playerLoc = game->player.space;

  // Camera Following
  if (!game->pause) {
    game->cameraPos.x += (playerLoc.posx - game->cameraPos.x) / 5;
    game->cameraPos.y += (playerLoc.posy - game->cameraPos.y) / 5;
  }

  renderBackground(layer, game);
  renderMap(layer, game->map, &game->cameraPos);

  renderPlayer(layer, game);
  renderStatus(layer, game);

  if (game->pause) {
    renderPauseScreen(layer, game);
  } else {
    updateGame(game);
  }
}

void renderBackground(int layer, Game *game) {
  int tex = getTextureId(game->texMap, "./images/background/01.jpg");
  double img_w, img_h, aspect;
  HgImageSize(tex, &img_w, &img_h);
  aspect = WINDOW_HEIGHT / WINDOW_WIDTH;
  double sx = game->cameraPos.x / WINDOW_WIDTH;
  double sy = game->cameraPos.y / WINDOW_HEIGHT;
  double sw = img_w;
  double sh = img_h * aspect;
  HgWImageDrawRect(layer, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, tex, sx, sy, sw,
                   sh);
}

void renderPlayer(int layer, Game *game) {
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

void renderPauseScreen(int layer, Game *game) {
  HgWSetFillColor(layer, HgRGBA(0, 0, 0, 0.75));
  HgWBoxFill(layer, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

  HgWSetColor(layer, HG_WHITE);
  HgWSetFont(layer, HG_GB, 36);
  renderTextCenter(layer, WINDOW_WIDTH / 2, WINDOW_HEIGHT - 100, "Pause");
}

void renderStatus(int layer, Game *game) {
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

Tile *getHitTile(Space2d *space, Map *map) {
  int left = LEFT(space);
  int right = RIGHT(space);
  int top = TOP(space);
  int bot = BOTTOM(space);

  Tile *tileLetfTop = getTileAt(map, left, top);
  Tile *tileLetBot = getTileAt(map, left, bot);
  Tile *tileRightTop = getTileAt(map, right, top);
  Tile *tileRightBot = getTileAt(map, right, bot);

  if (checkTileHit(space, tileLetfTop)) {
    return tileLetBot;
  } else if (checkTileHit(space, tileRightTop)) {
    return tileRightTop;
  } else if (checkTileHit(space, tileRightBot)) {
    return tileRightBot;
  } else if (checkTileHit(space, tileLetBot)) {
    return tileLetBot;
  }
  return NULL;
}

bool checkTileHit(Space2d *space, Tile *tile) {
  if (tile == NULL) {
    return false;
  }

  if (tile->type == TILE_AIR) {
    return false;
  }

  return true;
}

void result(int layer, Game *game) {}