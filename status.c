#include "status.h"
#include <stdio.h>

void initializeStatus(Status *s)
{
  s->money = 0;
}

void formatMoney(Status *status)
{
  int money = status->money;

  if (money >= 100000)
  {
    sprintf(status->moneyText, "%f", (double)money / 1000);
  }
  else
  {
    sprintf(status->moneyText, "%d", money);
  }
}