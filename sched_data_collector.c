// sched_data_collector.c
// Comprehensive data collection for scheduling algorithm comparison
#include "types.h"
#include "stat.h"
#include "user.h"

#define NPROCS 10
#define WORKLOAD 2000000

struct process_data {
    int process_id;
    int start_time;
    int end_time;
    int turnaround_time;
    int priority_or_tickets;
    char algorithm[20];
};

void log_to_csv(struct process_data *data, int count, char *algorithm) {
    int fd;
    char filename[50];

    // Create algorithm-specific filename
    strcpy(filename, algorithm);
    strcat(filename, "_performance.csv");

    fd = open(filename, O_CREATE | O_RDWR);
    if(fd < 0) {
        printf(1, "Error: Cannot create %s\n", filename);
        return;
    }

    // Write CSV header
    printf(fd, "Algorithm,Process_ID,Start_Time,End_Time,Turnaround_Time,Priority_Tickets,Workload\n");

    // Write data for each process
    for(int i = 0; i < count; i++) {
        printf(fd, "%s,%d,%d,%d,%d,%d,%d\n",
               data[i].algorithm,
               data[i].process_id,
               data[i].start_time,
               data[i].end_time,
               data[i].turnaround_time,
               data[i].priority_or_tickets,
               WORKLOAD);
    }

    // Calculate and write summary statistics
    int total_turnaround = 0;
    int min_turnaround = data[0].turnaround_time;
    int max_turnaround = data[0].turnaround_time;

    for(int i = 0; i < count; i++) {
        total_turnaround += data[i].turnaround_time;
        if(data[i].turnaround_time < min_turnaround)
            min_turnaround = data[i].turnaround_time;
        if(data[i].turnaround_time > max_turnaround)
            max_turnaround = data[i].turnaround_time;
    }

    printf(fd, "\nSummary Statistics:\n");
    printf(fd, "Average_Turnaround,%d\n", total_turnaround / count);
    printf(fd, "Min_Turnaround,%d\n", min_turnaround);
    printf(fd, "Max_Turnaround,%d\n", max_turnaround);
    printf(fd, "Total_Processes,%d\n", count);
    printf(fd, "Total_Time,%d\n", data[count-1].end_time - data[0].start_time);

    close(fd);
    printf(1, "Data logged to %s\n", filename);
}

int main(void) {
    struct process_data data[NPROCS];
    int i, pid;
    int global_start = uptime();

    // Determine which algorithm is active
    char *algorithm_name;
    #ifdef FCFS
    algorithm_name = "FCFS";
    #elif defined(Priority)
    algorithm_name = "Priority";
    #elif defined(Lottery)
    algorithm_name = "Lottery";
    #else
    algorithm_name = "RoundRobin";
    #endif

    printf(1, "Collecting performance data for %s scheduling\n", algorithm_name);
    printf(1, "Running %d processes with workload %d\n", NPROCS, WORKLOAD);

    // Fork processes and collect timing data
    for(i = 0; i < NPROCS; i++) {
        data[i].process_id = i;
        data[i].start_time = uptime();
        strcpy(data[i].algorithm, algorithm_name);

        pid = fork();
        if(pid == 0) {
            // Child process
            int child_start = uptime();

            // Set scheduling parameters
            #ifdef Priority
            int priority = 10 + (i % 5); // Priorities 10-14
            changepriority(getpid(), priority);
            data[i].priority_or_tickets = priority;
            #elif defined(Lottery)
            int tickets = 10 + (i * 5); // Tickets 10, 15, 20, 25, etc.
            setlotterytickets(getpid(), tickets);
            data[i].priority_or_tickets = tickets;
            #else
            data[i].priority_or_tickets = 0; // No priority/tickets for FCFS
            #endif

            // CPU-intensive workload
            volatile int x = 0;
            for(int j = 0; j < WORKLOAD; j++) {
                x = x + j * 3;
                if(j % 100000 == 0) {
                    // Occasional yield to make scheduling differences more visible
                    yield();
                }
            }

            int child_end = uptime();
            printf(1, "Process %d completed: %d ticks\n", i, child_end - child_start);
            exit();
        } else if(pid < 0) {
            printf(1, "Fork failed for process %d\n", i);
        }
    }

    // Wait for all children and record end times
    for(i = 0; i < NPROCS; i++) {
        wait();
        data[i].end_time = uptime();
        data[i].turnaround_time = data[i].end_time - data[i].start_time;
    }

    // Log data to CSV file
    log_to_csv(data, NPROCS, algorithm_name);

    // Also create a summary comparison file
    int summary_fd = open("algorithm_comparison.csv", O_CREATE | O_RDWR);
    if(summary_fd >= 0) {
        // Check if file is empty (first run)
        struct stat st;
        if(fstat(summary_fd, &st) == 0 && st.size == 0) {
            printf(summary_fd, "Algorithm,Avg_Turnaround,Min_Turnaround,Max_Turnaround,Total_Time,Throughput\n");
        }

        int total_turnaround = 0;
        int min_turnaround = data[0].turnaround_time;
        int max_turnaround = data[0].turnaround_time;

        for(i = 0; i < NPROCS; i++) {
            total_turnaround += data[i].turnaround_time;
            if(data[i].turnaround_time < min_turnaround)
                min_turnaround = data[i].turnaround_time;
            if(data[i].turnaround_time > max_turnaround)
                max_turnaround = data[i].turnaround_time;
        }

        int total_time = data[NPROCS-1].end_time - global_start;
        int avg_turnaround = total_turnaround / NPROCS;
        int throughput = (NPROCS * 100) / total_time;

        printf(summary_fd, "%s,%d,%d,%d,%d,%d\n",
               algorithm_name, avg_turnaround, min_turnaround, max_turnaround, total_time, throughput);
        close(summary_fd);
    }

    printf(1, "Performance data collection completed for %s\n", algorithm_name);
    printf(1, "Files generated: %s_performance.csv and algorithm_comparison.csv\n", algorithm_name);

    exit();
}
