#ifndef STATUS_H
#define STATUS_H

#define DEFAULT_AMMO 999
//ジャンプのクールタイム（フレーム）
#define DEFAULT_JUMP_CT 10

// @brief brief 発射のクールタイム（フレーム）
#define DEFAULT_GUN_CT 10

typedef struct {
  char name[32];

  int health;

  int score;

  int jumpCooltime;

  int ammoRemaining;

  int gunCooldown;

  int tickDead;

  int tickLived;
} Status;

void initializeStatus(Status *status);

void AddScore(Status *s, int amount);

void FormatScore(Status *status, char *s);

/**
 * @brief ジャンプ可能かどうかを調べる
 *
 * @param s
 * @return int
 */
int CanJump(Status *s);

/**
 * @brief ジャンプのクールタイム時間を設定する
 *
 * @param s
 * @param i
 */
void SetJumpCooltime(Status *s, int i);

/**
 * @brief 発射のクールタイムを設定する
 *
 * @param s
 * @param i
 */
void SetGunCooltime(Status *s, int i);

/**
 * @brief ダメージを与える
 *
 * @param s
 * @param amount
 */
void AddDamage(Status *s, int amount);

/**
 * @brief 生きている状態か判定する
 *
 * @param s
 * @return int
 */
int IsAlive(Status *s);

#endif
