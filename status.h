#ifndef STATUS_H
#define STATUS_H

typedef struct {
  char name[32];

  int score;

  int jumpCooltime;

  int ammoRemaining;

  int gunCooldown;
} Status;

void initializeStatus(Status *status);

void FormatScore(Status *status, char *s);

/**
 * @brief ジャンプ可能かどうかを調べる
 *
 * @param s
 * @return int
 */
int CanJump(Status *s);

void SetJumpCooltime(Status *s, int i);
#endif
