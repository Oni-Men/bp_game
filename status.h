#ifndef _DEF_STATUS_H_
#define _DEF_STATUS_H_

typedef struct
{
  char name[32];
  int money;
  char moneyText[32];
} Status;

void initializeStatus(Status *status);

void formatMoney(Status *status);
#endif
