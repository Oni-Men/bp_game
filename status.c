#include "status.h"

#include <stdio.h>

void initializeStatus(Status *s) {
  s->health = 10;
  s->score = 0;
  s->jumpCooltime = 0;
  s->ammoRemaining = 999;
  s->gunCooldown = 0;
  s->tickDead = -1;
  s->tickLived = 0;
}

void AddScore(Status *s, int amount) {
  s->score += amount;
  if (s->score < 0) {
    s->score = 0;
  }
}

void FormatScore(Status *status, char *s) {
  int score = status->score;

  if (score >= 100000) {
    sprintf(s, "%f", (double)score / 100000);
  } else {
    sprintf(s, "%d", score);
  }
}

int CanJump(Status *s) { return s->jumpCooltime == 0; }

void SetJumpCooltime(Status *s, int i) {
  if (i < 0) {
    i = 0;
  }
  s->jumpCooltime = i;
}

void SetGunCooltime(Status *s, int i) {
  if (i < 0) {
    i = 0;
  }
  s->gunCooldown = i;
}

void AddDamage(Status *s, int amount) {
  s->health -= amount;
  if (s->health < 0) {
    s->health = 0;
  }
}

int IsAlive(Status *s) { return (s->health > 0); }