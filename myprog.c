#include "types.h"
#include "fcntl.h"
#include "user.h"

struct proc_stat {
  int wtime, rtime, ctime, etime, iotime, restime; 
  #ifdef Lottery
  int tickets;
  #endif
  #ifdef Priority
  int priority;
  #endif
};

void save(struct proc_stat *p, int n) {
    int fd, i;
    int avgWait = 0, avgRun = 0, avgRes = 0, avgTat = 0;
    fd = open("../data.csv", O_CREATE | O_RDWR);
    if(fd >= 0)
        printf(1, "writing in data.csv\n");
    else {
        printf(1, "error: create data.csv failed\n");
        exit();
    }
    #ifdef Lottery
    printf(fd, "tickets,wait,run,creat,end,io,tat,res\n");
     #endif
    #ifdef Priority
     printf(fd, "Priority,wait,run,creat,end,io,tat,res\n");
     #endif
    #ifdef FCFS
    printf(fd, "wait,run,creat,end,io,tat,res\n");
    #endif
    for(i = 0; i < n; i++) {
      avgRes += (p+i)->restime;
      avgWait += (p+i)->wtime;
      avgRun += (p+i)->rtime;
      avgTat += (p+i)->etime - (p+i)->ctime;
      #ifdef Lottery
      printf(fd, "%d,%d,%d,%d,%d,%d,%d,%d\n", (p+i)->tickets, (p+i)->wtime, (p+i)->rtime, (p+i)->ctime, (p+i)->etime, (p+i)->iotime, (p+i)->etime-(p+i)->ctime, (p+i)->restime);
      #endif
      #ifdef Priority 
       printf(fd, "%d,%d,%d,%d,%d,%d,%d,%d\n", (p+i)->priority, (p+i)->wtime, (p+i)->rtime, (p+i)->ctime, (p+i)->etime, (p+i)->iotime, (p+i)->etime-(p+i)->ctime, (p+i)->restime);
      #endif  
      #ifdef FCFS
      printf(fd, "%d,%d,%d,%d,%d,%d,%d\n", (p+i)->wtime, (p+i)->rtime, (p+i)->ctime, (p+i)->etime, (p+i)->iotime, (p+i)->etime-(p+i)->ctime, (p+i)->restime);
      #endif
    }
    printf(fd, "avgWait,avgRun, avgRes\n");
    printf(fd, "%d,%d,%d\n", avgWait/n,avgRun/n, avgRes/n);
    printf(fd, "Throughput : %d processes/second\n", 100*n/avgRun);
    printf(1, "Throughput : %d processes/second\n", 100*n/avgRun);
    printf(1, "write done\n");
    close(fd);
}

int main(int argc, char *argv[])
{
  int processes = 0, limit = 3e7, ppid = 0;
  double z = 0, x = 0;
  if (argc != 2)
    processes = 10;
  else
    processes = atoi(argv[1]);
  
  struct proc_stat p[processes];
  #ifdef FCFS
  limit =3e7;
   for(int j = 0; j < processes; j++){
            ppid = fork();
            if(ppid == 0){
                for (z = 0; z < limit; z += 1)
                    x = x +
                        3.14 * 89.64;
                exit();
            }
            else if(ppid < 0){
                printf(1, "%d failed in fork!\n", getpid());
            }
   }
  
  #else
  for (int j = 0; j < processes; j++)
  {
    ppid = fork();
    if (ppid == 0)
    {
      int pid = getpid();
      for (z = 0; z < limit / 2; z += 1)
        x = x + 45.93 * 0.23;
      // 3.14 * 89.64; // useless calculations to consume CPU time
      #ifdef Lottery
      setlotterytickets(pid, 50 - pid / 2);
      #endif
      #ifdef Priority
      changepriority(pid, 50 - pid / 2);
      #endif
      
      for (z = 0; z < limit / 2; z += 1)
        x = x + 45.93 * 0.23;
      // 3.14 * 89.64; // useless calculations to consume CPU time
      exit();
    }
    else if (ppid < 0)
      printf(1, "%d failed in fork!\n", getpid());
  }

  #endif
  for (int i = 0; i < processes; i++)
  {
    #ifdef Lottery
    proc_stats(&p[i].wtime, &p[i].rtime, &p[i].ctime, &p[i].etime, &p[i].iotime, &p[i].restime, &p[i].tickets);
    #endif
    #ifdef Priority
    proc_stats(&p[i].wtime, &p[i].rtime, &p[i].ctime, &p[i].etime, &p[i].iotime, &p[i].restime, &p[i].priority);
    #endif
     #ifdef FCFS
    proc_stats(&p[i].wtime, &p[i].rtime, &p[i].ctime, &p[i].etime, &p[i].iotime, &p[i].restime);
    #endif
  }

  save(p, processes);
  exit();
}
