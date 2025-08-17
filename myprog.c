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

    #ifdef Lottery
    fd = open("lottery_data.csv", O_CREATE | O_RDWR);
    #endif
    #ifdef Priority
    fd = open("priority_data.csv", O_CREATE | O_RDWR);
    #endif
    #ifdef FCFS
    fd = open("fcfs_data.csv", O_CREATE | O_RDWR);
    #endif

    if(fd >= 0)
        printf(1, "writing scheduling data to CSV\n");
    else {
        printf(1, "error: create CSV file failed\n");
        exit();
    }

    #ifdef Lottery
    printf(fd, "algorithm,process_id,tickets,wait_time,run_time,creation_time,end_time,io_time,turnaround_time,response_time\n");
    #endif
    #ifdef Priority
    printf(fd, "algorithm,process_id,priority,wait_time,run_time,creation_time,end_time,io_time,turnaround_time,response_time\n");
    #endif
    #ifdef FCFS
    printf(fd, "algorithm,process_id,wait_time,run_time,creation_time,end_time,io_time,turnaround_time,response_time\n");
    #endif
    for(i = 0; i < n; i++) {
      avgRes += (p+i)->restime;
      avgWait += (p+i)->wtime;
      avgRun += (p+i)->rtime;
      avgTat += (p+i)->etime - (p+i)->ctime;

      #ifdef Lottery
      printf(fd, "Lottery,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", i, (p+i)->tickets, (p+i)->wtime, (p+i)->rtime, (p+i)->ctime, (p+i)->etime, (p+i)->iotime, (p+i)->etime-(p+i)->ctime, (p+i)->restime);
      #endif
      #ifdef Priority
      printf(fd, "Priority,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", i, (p+i)->priority, (p+i)->wtime, (p+i)->rtime, (p+i)->ctime, (p+i)->etime, (p+i)->iotime, (p+i)->etime-(p+i)->ctime, (p+i)->restime);
      #endif
      #ifdef FCFS
      printf(fd, "FCFS,%d,%d,%d,%d,%d,%d,%d,%d\n", i, (p+i)->wtime, (p+i)->rtime, (p+i)->ctime, (p+i)->etime, (p+i)->iotime, (p+i)->etime-(p+i)->ctime, (p+i)->restime);
      #endif
    }
    printf(fd, "\nSummary Statistics:\n");
    printf(fd, "Average Wait Time,%d\n", avgWait/n);
    printf(fd, "Average Run Time,%d\n", avgRun/n);
    printf(fd, "Average Response Time,%d\n", avgRes/n);
    printf(fd, "Average Turnaround Time,%d\n", avgTat/n);
    printf(fd, "Throughput,%d processes per 100 ticks\n", 100*n/avgRun);

    printf(1, "Algorithm Performance Summary:\n");
    printf(1, "Average Wait Time: %d ticks\n", avgWait/n);
    printf(1, "Average Turnaround Time: %d ticks\n", avgTat/n);
    printf(1, "Throughput: %d processes per 100 ticks\n", 100*n/avgRun);
    printf(1, "Data written to CSV file\n");
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
  // For Priority and Lottery scheduling
  for (int j = 0; j < processes; j++)
  {
    ppid = fork();
    if (ppid == 0)
    {
      int pid = getpid();

      // Set priority/tickets BEFORE starting work
      #ifdef Lottery
      setlotterytickets(pid, 10 + (j * 5)); // Different ticket counts: 10, 15, 20, 25, etc.
      #endif
      #ifdef Priority
      changepriority(pid, 10 + j); // Different priorities: 10, 11, 12, 13, etc.
      #endif

      // CPU-intensive work to test scheduling
      for (z = 0; z < limit; z += 1)
        x = x + 3.14 * 89.64;

      exit();
    }
    else if (ppid < 0)
      printf(1, "%d failed in fork!\n", getpid());
  }

  #endif

  // Wait for all child processes to complete
  for (int i = 0; i < processes; i++) {
    wait();
  }

  // Collect statistics for each completed process
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
