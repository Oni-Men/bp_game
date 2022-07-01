#ifndef STATUS_H
#define STATUS_H

typedef struct {
  char name[32];
  int money;
  char moneyText[32];
} Status;

void initializeStatus(Status *status);

void formatMoney(Status *status);
#endif
