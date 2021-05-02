#include "types.h"
#include "user.h"

int main(int argc, char *argv[])
{

  int pid;
  int waitid = 0, a = 0, b = 0, c = 0, d = 0, e = 0, f = 0, g = 0;
  pid = fork();
  if (pid == 0)
  {
    exec(argv[1], argv);
    printf(1, "exec %s failed\n", argv[1]);
  }
  else
  {
    waitid = proc_stats(&a, &b, &c, &d, &e, &f, &g);
  }
  printf(1, "child process %d\n Priority %d\nWait Time = %d\nRun Time = %d\nCreation Time = %d\nEnd Time = %d \n \nIO Time = %d\nResponse Time = %d\n", waitid, g, a, b, c, d, e, f);
  exit();
}