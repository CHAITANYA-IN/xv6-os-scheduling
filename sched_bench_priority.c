// sched_bench_priority.c
// Benchmark for Preemptive Priority scheduling in xv6
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#define NPROCS 5
#define WORKLOAD 10000000

int main(void) {
  int i;
  int start = uptime();
  int completion_times[NPROCS];
  int priorities[NPROCS];

  printf(1, "Testing Priority Scheduling (if enabled)\n");

  // Open CSV file for logging
  int fd = open("priority_bench_data.csv", O_CREATE | O_RDWR);
  if(fd >= 0) {
    printf(fd, "Process_ID,Priority,Start_Time,Completion_Time,Turnaround_Time\n");
  }

  for(i = 0; i < NPROCS; i++) {
    if(fork() == 0) {
      int child_start = uptime();
      int priority = NPROCS - i;

      #ifdef Priority
      changepriority(getpid(), priority);
      printf(1, "Process %d: Set priority to %d\n", i, priority);
      #else
      printf(1, "Process %d: Priority scheduling not enabled\n", i);
      #endif

      int j;
      for(j = 0; j < WORKLOAD; j++) {
        asm volatile(""); // Busy work
      }

      int child_end = uptime();
      printf(1, "Priority: Child %d finished at %d ticks (turnaround: %d)\n",
             i, child_end, child_end - child_start);

      // Log to file
      if(fd >= 0) {
        printf(fd, "%d,%d,%d,%d,%d\n", i, priority, child_start, child_end, child_end - child_start);
      }

      exit();
    }
  }

  for(i = 0; i < NPROCS; i++) wait();

  int end = uptime();
  printf(1, "Priority: Total time for %d procs: %d ticks\n", NPROCS, end - start);

  // Log summary
  if(fd >= 0) {
    printf(fd, "\nSummary:\n");
    printf(fd, "Total_Processes,%d\n", NPROCS);
    printf(fd, "Total_Time,%d\n", end - start);
    printf(fd, "Throughput,%d\n", (NPROCS * 100) / (end - start));
    close(fd);
    printf(1, "Data logged to priority_bench_data.csv\n");
  }

  exit();
}
