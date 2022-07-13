#include "game.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "input.h"
#include "timeutil.h"
#include "words.h"

#define MAX_FALLING_SPEED (-10)

int rand_counter = 0;

double range_randf(double min, double max) {
  srand(rand_counter);
  rand_counter += 100;
  return (((double)rand() / RAND_MAX) * (max - min) + min);
}

void initGame(Game *game) {
  printf("section start: initGame\n");
  SetGameState(game, TITLE);
  game->showDebug = false;
  game->useDoubleLayer = true;
  game->timeAtLastFrame = milliSecAtNow();
  game->exit = false;
  game->pause = false;
  game->lastEnemySpawned = milliSecAtNow();

  TextureMap *texMap = (TextureMap *)malloc(sizeof(TextureMap));
  initTextureMap(texMap);
  game->texMap = texMap;

  game->cameraPos.x = 0;
  game->cameraPos.y = 0;

  printf("section end: initGame\n");
}

void UpdateGame(Game *game) {
  int now = milliSecAtNow();
  Entity *player = &(game->player);
  player->status.tickLived++;

  if (isKeyDown('a')) {
    player->space.posx -= 4;
  }
  if (isKeyDown('d')) {
    player->space.posx += 4;
  }
  if (isKeyDown('w') && CanJump(&player->status)) {
    player->velocity.y = 3.5;
    SetJumpCooltime(&player->status, 300);
  }
  if (isKeyDown('s')) {
    player->velocity.y -= 2.5;
  }
  if (isKeyDown(0x20)) {
    PlayAct(game);
  }

  // 最大落下速度を超えたら、落下速度を抑える力を加える
  player->velocity.y -= 0.1;
  if (player->velocity.y < MAX_FALLING_SPEED) {
    player->velocity.y += (MAX_FALLING_SPEED - player->velocity.y) / 5.0;
  }

  // プレイヤーを速度ベクトル分移動
  player->space.posx += player->velocity.x;
  player->space.posy += player->velocity.y;

  if (player->space.posy < 0) {
    SetGameState(game, RESULT);
  }

  // ブーストのクールタイムを更新
  if (player->status.jumpCooltime > 0) {
    player->status.jumpCooltime--;
  }

  // 発射のクールタイム期間を更新
  if (player->status.gunCooldown > 0) {
    player->status.gunCooldown--;
  }

  UpdateBullets(game);
  UpdateEnemies(game);

  double windowW, windowH;
  HgGetSize(&windowW, &windowH);

  Entity *e;
  e = malloc(sizeof(Entity));
  if (e == NULL) {
    printf("Failed to allocate the memory for the Entity.\n");
    exit(1);
  }
  if (now - game->lastEnemySpawned > 500) {
    initializeStatus(&e->status);
    e->space.posx = player->space.posx +
                    ((int)player->space.posy % (int)windowW) - windowW / 2;
    e->space.posy = player->space.posy - windowH / 2;
    e->space.width = 40;
    e->space.height = 40;
    AddEnemy(game, e);
    game->lastEnemySpawned = now;
  }

  if (!IsAlive(&player->status)) {
    SetGameState(game, RESULT);
  }
}

void PlayAct(Game *game) {
  Entity *p = &game->player;
  Status *s = &game->player.status;
  // 残弾数が0以下または射撃クールダウン状態のときは射撃できない
  if (s->ammoRemaining <= 0 || s->gunCooldown > 0) {
    return;
  }
  s->ammoRemaining--;
  s->gunCooldown = 20;

  double window_w, window_h;
  HgGetSize(&window_w, &window_h);

  //射出するワードを取得
  // char *word = GetRandomWord();

  //プレイヤーの中心座標
  double x = CENTER_X((&p->space));
  double y = CENTER_Y((&p->space));

  //マウス位置のゲーム座標
  double tx = getMouseX() + game->cameraPos.x - window_w / 2;
  double ty = getMouseY() + game->cameraPos.y - window_h / 2;

  //プレイヤー位置とマウス位置を結んだ線分がX軸となす角度（ラジアン）
  double rot = atan2f(ty - y, tx - x);

  //それぞれの弾丸の角度（ラジアン）
  double angle;

  // 弾丸の発射処理
  int len = 10;
  for (int i = 0; i < len; i++) {
    Entity *e = (Entity *)malloc(sizeof(Entity));
    if (e == NULL) {
      printf("Unable to allocate the memory for a bullet.\n");
      continue;
    }
    e->status.health = 1;
    e->space.width = BULLET_SIZE;
    e->space.height = BULLET_SIZE;

    e->space.posx = x - BULLET_SIZE / 2;
    e->space.posy = y - BULLET_SIZE / 2;

    if (rot < 0) {
      angle = (i - len * 0.5) * M_PI / 36.0;

    } else {
      angle = (-i + len * 0.5) * M_PI / 36.0;
    }
    e->rotation = rot + angle;
    e->velocity = p->velocity;
    e->velocity.x += 5 * cos(e->rotation);
    e->velocity.y += 5 * sin(e->rotation);

    AddBullet(game, e);
  }
}

void AddBullet(Game *game, Entity *bullet) {
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (game->bullets[i] != NULL) {
      continue;
    }
    bullet->id = i;
    game->bullets[i] = bullet;
    break;
  }
}

void RemoveBullet(Game *game, Entity *b) {
  int id = b->id;
  if (id < 0 || id >= MAX_BULLETS) {
    return;
  }
  if (game->bullets[id] == NULL) {
    return;
  }
  free(b);
  b = NULL;
  game->bullets[id] = NULL;
}

void UpdateBullets(Game *game) {
  double width, height, windowDiagSq;
  Entity *p = &game->player;
  Entity *e;

  //ウィンドウサイズを取得
  HgGetSize(&width, &height);
  windowDiagSq = width * width + height * height;
  game->bulletsCount = 0;

  //すべての弾丸を処理する
  for (int i = 0; i < MAX_BULLETS; i++) {
    e = game->bullets[i];
    if (e == NULL) {
      continue;
    }

    game->bulletsCount++;

    if (!IsAlive(&e->status)) {
      e->status.tickDead++;
      continue;
    }
    e->status.tickLived++;

    e->velocity.y -= 0.05;
    e->space.posx += e->velocity.x;
    e->space.posy += e->velocity.y;

    double diagSq, dx, dy, distSq;

    //プレイヤーと弾丸の距離が、「画面の対角線の長さ」を超えたら削除する
    dx = p->space.posx - e->space.posx;
    dy = p->space.posy - e->space.posy;
    distSq = dx * dx + dy * dy;
    if (distSq > windowDiagSq) {
      RemoveBullet(game, e);
      continue;
    }

    //この弾丸と、すべての敵のあたり判定処理をする
    Entity *enemy;
    for (int k = 0; k < MAX_ENEMIES; k++) {
      enemy = game->enemies[k];
      if (enemy == NULL) {
        continue;
      }

      //弾丸と敵の距離が、「それぞれの対角線を足した長さ」より近ければ当たっているとみなす
      dx = CENTER_X((&enemy->space)) - CENTER_X((&e->space));
      dy = CENTER_Y((&enemy->space)) - CENTER_Y((&e->space));
      distSq = dx * dx + dy * dy;
      diagSq = enemy->space.width * enemy->space.width +
               enemy->space.height * enemy->space.height;
      if (distSq < BULLET_SIZE * BULLET_SIZE + diagSq) {
        RemoveBullet(game, e);
        RemoveEnemy(game, enemy);
        AddScore(&p->status, 1);
      }
    }
  }
}

void AddEnemy(Game *game, Entity *e) {
  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (game->enemies[i] != NULL) {
      continue;
    }
    e->id = i;
    game->enemies[i] = e;
    break;
  }
}

void RemoveEnemy(Game *game, Entity *e) {
  int id = e->id;
  if (id < 0 || id >= MAX_ENEMIES) {
    return;
  }

  if (game->enemies[id] == NULL) {
    return;
  }
  free(e);
  e = NULL;
  game->enemies[id] = NULL;
}

void UpdateEnemies(Game *game) {
  int now = milliSecAtNow();
  double width, height, windowDiagSq;
  double dx, dy, distSq, diagSq;
  Entity *p = &game->player;
  Entity *e;

  //ウィンドウサイズを取得
  HgGetSize(&width, &height);
  //ウィンドウの対角線の長さの二乗
  windowDiagSq = width * width + height * height;

  game->enemyCount = 0;
  for (int i = 0; i < MAX_ENEMIES; i++) {
    e = game->enemies[i];
    if (e == NULL) {
      continue;
    }

    game->enemyCount++;

    if (!IsAlive(&e->status)) {
      e->status.tickDead++;
      continue;
    }

    e->status.tickLived++;
    e->velocity.y -= 0.03;

    //最大落下速度を超えないようにする
    if (e->velocity.y < MAX_FALLING_SPEED / 2) {
      e->velocity.y = MAX_FALLING_SPEED / 2;
    }

    e->space.posx += e->velocity.x;
    e->space.posy += e->velocity.y;
    e->rotation += 0.01;

    dx = p->space.posx - e->space.posx;
    dy = p->space.posy - e->space.posy;
    distSq = dx * dx + dy * dy;
    diagSq =
        e->space.width * e->space.width + e->space.height * e->space.height;
    //プレイヤーと敵の距離が、「ウィンドウの対角線と敵のサイズの対角線を足した長さ」を超えたら敵を削除
    if (distSq > windowDiagSq + diagSq) {
      RemoveEnemy(game, e);
    }

    if (distSq < diagSq / 2) {
      AddDamage(&p->status, 1);
      RemoveEnemy(game, e);
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
  game->cameraPos.x = 0;
  game->cameraPos.y = 0;
  printf("section end: initTitle\n");
}

void initPlay(Game *game) {
  printf("section start: initPlay\n");
  game->cameraPos.x = 0;
  game->cameraPos.y = 0;

  Entity *player = &game->player;
  player->space.posx = 0;
  player->space.posy = FIELD_HEIGHT;

  for (int i = 0; i < MAX_BULLETS; i++) {
    game->bullets[i] = NULL;
  }

  for (int i = 0; i < MAX_ENEMIES; i++) {
    game->enemies[i] = NULL;
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

  ButtonList list3 = NewButtonList();
  Space2d box3 = NewSpace(width / 2 - 150, 100, 300, 50);
  list3 = AddButton(list3, 0, box3, "メニューに戻る");
  game->buttons[RESULT] = list3;

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