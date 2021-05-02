#include "types.h"
#include "fcntl.h"
#include "user.h"

struct proc_stat {
  int wtime, rtime, ctime, etime, iotime, restime, tickets;
} p;

void save(struct proc_stat *p, int n) {
    int fd, i;
    fd = open("../data.csv", O_CREATE | O_RDWR);
    if(fd >= 0)
        printf(1, "writing in data.csv\n");
    else {
        printf(1, "error: create data.csv failed\n");
        exit();
    }
    printf(fd, "tickets,wait,run,creat,end,io,tat,res\n");
    for(i = 0; i < n; i++) {
      printf(fd, "%d,%d,%d,%d,%d,%d,%d,%d\n", (p+i)->tickets, (p+i)->wtime, (p+i)->rtime, (p+i)->ctime, (p+i)->etime, (p+i)->iotime, (p+i)->etime-(p+i)->ctime, (p+i)->restime);
    }
    printf(1, "write done\n");
    close(fd);
}

int main(int argc, char *argv[])
{
  int child = 0, limit = 3e7, ppid = 0;
  double z = 0, x = 0;
  if (argc != 2)
    child = 10;
  else
    child = atoi(argv[1]);
  
  struct proc_stat p[child];
  for (int j = 0; j < child; j++)
  {
    ppid = fork();
    if (ppid == 0)
    {
      int pid = getpid();
      for (z = 0; z < limit / 2; z += 1)
        x = x + 45.93 * 0.23;
      // 3.14 * 89.64; // useless calculations to consume CPU time
      setlotterytickets(pid, 50 - pid / 2);
      for (z = 0; z < limit / 2; z += 1)
        x = x + 45.93 * 0.23;
      // 3.14 * 89.64; // useless calculations to consume CPU time
      exit();
    }
    else if (ppid < 0)
      printf(1, "%d failed in fork!\n", getpid());
  }
  for (int i = 0; i < child; i++)
  {
    proc_stats(&p[i].wtime, &p[i].rtime, &p[i].ctime, &p[i].etime, &p[i].iotime, &p[i].restime, &p[i].tickets);
  }
  save(p, child);
  exit();
}
