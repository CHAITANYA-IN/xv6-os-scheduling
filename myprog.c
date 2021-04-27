#include "types.h"
#include "user.h"

void useLessLoop()
{
  double a = 0;
  while (a < 4000000)
    a += 0.07;
}

int main()
{
  int n1, n2, tickets;
  n1 = fork();
  n2 = fork();
  tickets = n1 ? (n2 ? 30 : 10) : 20;
  setlotterytickets(getpid(), tickets);
  if (!n2 && !n1)
    useLessLoop();
  if (n1 && n2)
    printf(1, "Process %d has child %d that is over!\n", getpid(), wait());
  printf(1, "Process %d has child %d that is over!\n", getpid(), wait());
  printf(1, "Tickets : %d => Process %d over!\n", tickets, getpid());
  exit();
}