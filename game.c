#include "game.h"

#include <stdbool.h>

void initGame(Game *game) {
  game->timeAtLastFrame = currentTimeMillis();
  game->state = STATE_TITLE;
  game->exit = false;
  game->pause = false;

  game->map = loadMap("./assets/map.txt");

  TextureMap *texMap = malloc(sizeof(TextureMap));
  initTextureMap(texMap);
  game->texMap = texMap;

  Entity *player = &game->player;
  player->space.posx = game->map->width / 2 * TILE_SIZE;
  player->space.posy = game->map->height * TILE_SIZE;
  player->hitTile = 0;
  player->onGround = false;

  initializeStatus(&player->status);
}

/**
 * @brief Process the game frame
 *
 * @param game
 */
void updateGame(Game *game) {
  Entity *player = &(game->player);
  if (isKeyPressed('a')) {
    player->space.posx -= 2;
  }
  if (isKeyPressed('d')) {
    player->space.posx += 2;
  }
  if (isKeyPressed('w')) {
    player->velocity.y = 5;
  }
  if (isKeyPressed('s')) {
    player->velocity.y = -10;
  }

  Space2d tickedSpace = player->space;
  Move(&tickedSpace, player->velocity.x, player->velocity.y);

  Tile *hitTile = getHitTile(&tickedSpace, game->map);
  player->hitTile = hitTile;
  Map *map = game->map;

  if (hitTile != NULL) {
    Space2dHitResult res = CheckBoxHit(&tickedSpace, &hitTile->space);
    if (!res.hitX) {
      player->space.posx += player->velocity.x;
    } else if (!res.hitY) {
      player->space.posy += player->velocity.y;
    }
  } else {
    player->velocity.y -= 0.1;
    player->space.posx += player->velocity.x;
    player->space.posy += player->velocity.y;
  }

  if (player->space.posy < 0) {
    player->space.posy = 0;
  }
}

void uodateButtons(Game *game, int state, double width, double height) {
  ButtonList *buttons = &game->buttonsList[state];
  if (state == STATE_TITLE) {
    AddButton(buttons, width / 2 - 150, height / 2 + 30, 300, 50, "開始する");
    AddButton(buttons, width / 2 - 150, height / 2 + 30, 300, 50, "終了する");
  }

  if (state == STATE_PLAY) {
  }

  if (state == STATE_RESULT) {
  }
}

ButtonList *getButtonList(Game *game) {
  return &game->buttonsList[game->state];
}