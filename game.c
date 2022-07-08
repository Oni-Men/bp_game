#include "game.h"

#include <string.h>

#include "input.h"
#include "timeutil.h"
#include "words.h"

#define MAX_FALLING_SPEED (-10)

void initGame(Game *game) {
  printf("section start: initGame\n");
  SetGameState(game, TITLE);
  game->showDebug = false;
  game->useDoubleLayer = true;
  game->timeAtLastFrame = milliSecAtNow();
  game->exit = false;
  game->pause = false;

  game->map = loadMap("./assets/map.txt");

  TextureMap *texMap = malloc(sizeof(TextureMap));
  initTextureMap(texMap);
  game->texMap = texMap;

  game->cameraPos.x = 0;
  game->cameraPos.y = 0;

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
  if (isKeyDown('w') && CanJump(&player->status)) {
    player->velocity.y = 3.5;
    SetJumpCooltime(&player->status, 300);
  }
  if (isKeyDown('s')) {
    player->velocity.y = MAX_FALLING_SPEED;
  }
  if (isKeyDown(0x20)) {
    PlayAct(game);
  }

  Space2d tickedSpace = player->space;
  Move(&tickedSpace, player->velocity.x, player->velocity.y);

  player->velocity.y -= 0.1;
  if (player->velocity.y < MAX_FALLING_SPEED) {
    player->velocity.y = MAX_FALLING_SPEED;
  }
  player->space.posx += player->velocity.x;
  player->space.posy += player->velocity.y;

  if (player->space.posy < 0) {
    SetGameState(game, RESULT);
  }

  if (player->status.jumpCooltime > 0) {
    player->status.jumpCooltime--;
  }

  if (player->status.gunCooldown > 0) {
    player->status.gunCooldown--;
  }

  UpdateBullets(game);
}

void PlayAct(Game *game) {
  Entity *p = &game->player;
  Status *s = &game->player.status;
  if (s->ammoRemaining < 0 || s->gunCooldown > 0) {
    return;
  }
  s->ammoRemaining--;
  s->gunCooldown = 10;

  double window_w, window_h;
  HgGetSize(&window_w, &window_h);

  char *word = GetRandomWord();
  int word_len = (int)strlen(word);

  double x = p->space.posx + p->space.width / 2;
  double y = p->space.posy + p->space.height / 2;
  double tx = getMouseX() + game->cameraPos.x - window_w / 2;
  double ty = getMouseY() + game->cameraPos.y - window_h / 2;
  double rot = atan2f(ty - y, tx - x);
  double angle;

  for (int i = 0; i < word_len; i += 3) {
    Bullet *b = (Bullet *)malloc(sizeof(Entity));
    Entity *e = (Entity *)malloc(sizeof(Entity));
    e->space.width = BULLET_SIZE;
    e->space.height = BULLET_SIZE;

    e->space.posx = x + BULLET_SIZE / 2;
    e->space.posy = y + BULLET_SIZE / 2;

    if (rot < 0) {
      angle = (i - word_len * 0.5) * M_PI / 36.0;

    } else {
      angle = (-i + word_len * 0.5) * M_PI / 36.0;
    }
    e->rotation = rot + angle;
    e->velocity = p->velocity;
    e->velocity.x += 10 * cos(e->rotation);
    e->velocity.y += 2.5 * sin(e->rotation);

    b->s = malloc(3 * sizeof(char));
    b->s[0] = word[i];
    b->s[1] = word[i + 1];
    b->s[2] = word[i + 2];

    b->e = e;
    AddBullet(game, b);
  }
}

void AddBullet(Game *game, Bullet *bullet) {
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (game->bullets[i] != NULL) {
      continue;
    }
    bullet->id = i;
    game->bullets[i] = bullet;
    break;
  }
}

void RemoveBullet(Game *game, Bullet *b) {
  int id = b->id;
  if (id < 0 || id >= MAX_BULLETS) {
    return;
  }
  game->bullets[id] = NULL;
  free(b->e);
  free(b);
}

void UpdateBullets(Game *game) {
  double width, height;
  Bullet *bullet;
  Entity *p = &game->player;
  Entity *e;

  HgGetSize(&width, &height);
  game->bulletsCount = 0;
  for (int i = 0; i < MAX_BULLETS; i++) {
    bullet = game->bullets[i];
    if (bullet == NULL) {
      continue;
    }

    e = bullet->e;
    game->bulletsCount++;

    e->velocity.y -= 0.05;
    e->space.posx += e->velocity.x;
    e->space.posy += e->velocity.y;

    double a = p->space.posx - e->space.posx;
    double b = p->space.posy - e->space.posy;
    double distSq = (a * a) + (b * b);
    if (distSq > width * width && distSq > height * height) {
      RemoveBullet(game, bullet);
    }
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
    case TITLE:
      initTitle(game);
      break;
    case PLAY:
      initPlay(game);
      break;
    case RESULT:
      initResult(game);
      break;
    case EXIT:
      initExit(game);
      break;
    default:
      break;
  }

  // initButtons(game, width, height);
}

void initTitle(Game *game) {
  printf("section start: initTitle\n");
  game->cameraPos.y = 0;
  printf("section end: initTitle\n");
}

void initPlay(Game *game) {
  printf("section start: initPlay\n");
  game->cameraPos.x = 0;
  game->cameraPos.y = 0;

  Entity *player = &game->player;
  player->space.posx = 0;
  player->space.posy = 25000;
  player->hitTile = 0;
  player->onGround = false;

  for (int i = 0; i < MAX_BULLETS; i++) {
    game->bullets[i] = NULL;
  }

  initializeStatus(&player->status);

  printf("section end: initPlay\n");
}

void initResult(Game *game) {
  printf("section start: initResult\n");
  printf("section end : initResult\n");
}

void initExit(Game *game) {
  printf("section start: initExit\n");
  game->useDoubleLayer = false;
  printf("section end: initExit\n");
}

void initButtons(Game *game, double width, double height) {
  printf("section start: initButtons\n");

  ButtonList list1 = NewButtonList();
  Space2d box1 = NewSpace(width / 2 - 150, height / 2 + 30, 300, 50);
  Space2d box2 = NewSpace(width / 2 - 150, height / 2 - 30, 300, 50);
  list1 = AddButton(list1, 0, box1, "はじめる");
  list1 = AddButton(list1, 1, box2, "おわる");
  game->buttons[TITLE] = list1;

  ButtonList list2 = NewButtonList();
  list2 = AddButton(list2, 0, box1, "ゲームにもどる");
  list2 = AddButton(list2, 1, box2, "メニューにもどる");
  game->buttons[PLAY] = list2;

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