#include "game.h"

#include "input.h"
#include "timeutil.h"

void initGame(Game *game) {
  printf("section start: initGame\n");
  SetGameState(game, STATE_TITLE);
  game->showDebug = false;
  game->timeAtLastFrame = milliSecAtNow();
  game->exit = false;
  game->pause = false;

  game->map = loadMap("./assets/map.txt");

  // @button
  // game->buttonsList = NewButtonList(3);

  TextureMap *texMap = malloc(sizeof(TextureMap));
  initTextureMap(texMap);
  game->texMap = texMap;

  game->cameraPos.x = 0;
  game->cameraPos.y = 0;

  Entity *player = &game->player;
  player->space.posx = game->map->width / 2 * TILE_SIZE;
  player->space.posy = game->map->height * TILE_SIZE;
  player->hitTile = 0;
  player->onGround = false;

  initializeStatus(&player->status);

  printf("section end: initGame\n");
}

void UpdateGame(Game *game) {
  Entity *player = &(game->player);
  if (isKeyDown('a')) {
    player->space.posx -= 2;
  }
  if (isKeyDown('d')) {
    player->space.posx += 2;
  }
  if (isKeyDown('w')) {
    player->velocity.y = 5;
  }
  if (isKeyDown('s')) {
    player->velocity.y = -10;
  }

  Space2d tickedSpace = player->space;
  Move(&tickedSpace, player->velocity.x, player->velocity.y);

  player->velocity.y -= 0.1;
  if (player->velocity.y < -5) {
    player->velocity.y = -5;
  }
  player->space.posx += player->velocity.x;
  player->space.posy += player->velocity.y;

  if (player->space.posy < 0) {
    SetGameState(game, STATE_RESULT);
  }
}

void SetGameState(Game *game, int state) {
  if (game->state == state) {
    return;
  }
  game->state = state;

  double width, height;
  HgGetSize(&width, &height);

  switch (game->state) {
    case STATE_TITLE:
      initTitle(game);
      break;
    case STATE_PLAY:
      initPlay(game);
      break;
    default:
      break;
  }

  // initButtons(game, width, height);
}

void initTitle(Game *game) { game->cameraPos.y = 0; }

void initPlay(Game *game) {
  game->player.space.posx = 0;
  game->player.space.posy = 50000;

  game->cameraPos.x = 0;
  game->cameraPos.y = 0;
}

void initButtons(Game *game, double width, double height) {
  printf("section start: initButtons\n");

  ButtonList list = NewButtonList();
  Space2d box1 = NewSpace(width / 2 - 150, height / 2 + 30, 300, 50);
  Space2d box2 = NewSpace(width / 2 - 150, height / 2 - 30, 300, 50);
  list = AddButton(list, 0, box1, "開始する");
  list = AddButton(list, 1, box2, "終了する");
  game->buttons[STATE_TITLE] = list;

  printf("section end: initButtons\n");
}

ButtonList getButtonList(Game *game) {
  int state = game->state;
  int size = sizeof(game->buttons) / sizeof(ButtonList);
  if (state < 0 || state >= size) {
    return NULL;
  }
  return game->buttons[game->state];
}

int handleButtons(Game *game) {
  ButtonList buttons = getButtonList(game);
  if (buttons == NULL) {
    return -1;
  }

  if (!isMouseDown()) {
    return -1;
  }

  Button *button = GetHoveredButton(buttons, getMouseX(), getMouseY());
  if (button == NULL) {
    return -1;
  }

  // button->timeHovered++;

  // ButtonList list = buttons;
  // do {
  //   if (list->val != button && list->val != NULL) {
  //     list->val->timeHovered = 0;
  //   }
  //   list = list->next;
  // } while (list);

  // do {
  //   if (list->val != button && list->val != NULL) {
  //     list->val->timePressed = 0;
  //   }
  //   list = list->next;
  // } while (list);

  // button->timePressed++;

  return button->id;
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