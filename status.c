#include "status.h"

#include <stdio.h>

void initializeStatus(Status *s) {
  s->score = 0;
  s->jumpCooltime = 0;
  s->ammoRemaining = 999;
  s->gunCooldown = 0;
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