#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"

int sys_fork(void)
{
  return fork();
}

int sys_exit(void)
{
  exit();
  return 0; // not reached
}

int sys_wait(void)
{
  return wait();
}

int sys_kill(void)
{
  int pid;

  if (argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int sys_getpid(void)
{
  return myproc()->pid;
}

int sys_sbrk(void)
{
  int addr;
  int n;

  if (argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if (growproc(n) < 0)
    return -1;
  return addr;
}

int sys_sleep(void)
{
  int n;
  uint ticks0;

  if (argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n)
  {
    if (myproc()->killed)
    {
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
#ifdef Lottery
int sys_setlotterytickets(void)
{
  int pid, n;
  if (argint(0, &pid) < 0)
    return -1;
  if (argint(1, &n) < 0)
    return -2;
  if (pid < 1 || n < 1)
    return -3;
  return setlotterytickets(pid, n);
}
#endif
#ifdef Priority
int 
sys_changepriority(void) 
{
  int pid, priority;
  if(argint(0, &pid) == -1 || argint(1, &priority) == -1) 
    return -1;
  if(priority < 1)
    priority = 1;
  if(priority > 50)
    priority = 50;
  return changepriority(pid, priority);
}
#endif

int sys_ps(void) 
{
  return ps();
}
int sys_proc_stats(void) {
  int *waitTime, *runTime, *creationTime, *endTime, *ioTime, *resTime; 
  int *var=0;
  if(argptr(0, (char**)&waitTime, sizeof(int)) < 0)
    return -1;
  if(argptr(1, (char**)&runTime, sizeof(int)) < 0)
    return -1;
  if(argptr(2, (char**)&creationTime, sizeof(int)) < 0)
    return -1;
  if(argptr(3, (char**)&endTime, sizeof(int)) < 0)
    return -1;
  if(argptr(4, (char**)&ioTime, sizeof(int)) < 0)
    return -1;
  if(argptr(5, (char**)&resTime, sizeof(int)) < 0)
    return -1;
  #ifdef Lottery
  if(argptr(6, (char**)&var, sizeof(int)) < 0)
    return -1;
  return proc_stats(waitTime, runTime, creationTime, endTime, ioTime, resTime, var);
  #endif
  #ifdef Priority
  if(argptr(6, (char**)&var, sizeof(int)) < 0)
    return -1;
  return proc_stats(waitTime, runTime, creationTime, endTime, ioTime, resTime, var);
  #endif
    #ifdef FCFS
  return proc_stats(waitTime, runTime, creationTime, endTime, ioTime, resTime,var);
  #endif
}

int 
sys_halt(void) {
  char *p = "shutdown";
  for( ; *p; p++)
    outb(0xf4, 0x00);
  return 0;
}