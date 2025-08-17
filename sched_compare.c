// sched_compare.c
// Unified benchmark to compare all scheduling algorithms
#include "types.h"
#include "stat.h"
#include "user.h"

#define NPROCS 10
#define WORKLOAD 5000000

struct test_result {
    int algorithm_type; // 0=FCFS, 1=Priority, 2=Lottery
    int total_time;
    int avg_turnaround;
    int throughput;
};

void run_test(char* algo_name, int test_type) {
    int i, pid;
    int start_time = uptime();
    int completion_times[NPROCS];
    int processes_completed = 0;

    printf(1, "\n=== Testing %s Scheduling ===\n", algo_name);

    // Fork processes for testing
    for(i = 0; i < NPROCS; i++) {
        pid = fork();
        if(pid == 0) {
            // Child process
            int child_start = uptime();

            // Set scheduling parameters based on test type
            if(test_type == 1) { // Priority
                #ifdef Priority
                changepriority(getpid(), 10 + (i % 5)); // Priorities 10-14
                #endif
            } else if(test_type == 2) { // Lottery
                #ifdef Lottery
                setlotterytickets(getpid(), 10 + (i * 5)); // Tickets 10, 15, 20, etc.
                #endif
            }
            // For FCFS (test_type == 0), no special setup needed

            // CPU-intensive workload
            volatile int x = 0;
            for(int j = 0; j < WORKLOAD; j++) {
                x = x + j * 3;
            }

            int child_end = uptime();
            printf(1, "%s: Process %d completed in %d ticks\n",
                   algo_name, i, child_end - child_start);
            exit();
        } else if(pid < 0) {
            printf(1, "Fork failed for process %d\n", i);
        }
    }

    // Wait for all children and collect completion times
    for(i = 0; i < NPROCS; i++) {
        wait();
        processes_completed++;
    }

    int end_time = uptime();
    int total_time = end_time - start_time;

    printf(1, "%s Results:\n", algo_name);
    printf(1, "Total time: %d ticks\n", total_time);
    printf(1, "Processes completed: %d\n", processes_completed);
    printf(1, "Throughput: %d processes per 100 ticks\n", (processes_completed * 100) / total_time);
    printf(1, "================================\n");

    // Save results to file
    int fd = open("comparison_results.txt", O_CREATE | O_WRONLY);
    if(fd >= 0) {
        printf(fd, "%s,%d,%d,%d\n", algo_name, total_time, processes_completed, (processes_completed * 100) / total_time);
        close(fd);
    }
}

int main(void) {
    printf(1, "Starting Scheduler Comparison Test\n");
    printf(1, "Testing with %d processes, workload %d\n", NPROCS, WORKLOAD);

    // Create/clear results file
    int fd = open("comparison_results.txt", O_CREATE | O_WRONLY);
    if(fd >= 0) {
        printf(fd, "Algorithm,Total_Time,Processes_Completed,Throughput\n");
        close(fd);
    }

    // Test current scheduling algorithm
    #ifdef FCFS
    run_test("FCFS", 0);
    #endif

    #ifdef Priority
    run_test("Priority", 1);
    #endif

    #ifdef Lottery
    run_test("Lottery", 2);
    #endif

    // If no algorithm is specifically defined, assume round-robin/default
    #if !defined(FCFS) && !defined(Priority) && !defined(Lottery)
    run_test("Default_RoundRobin", 0);
    #endif

    printf(1, "Test completed. Results saved to comparison_results.txt\n");
    exit();
}
