#ifdef Priority
#include "types.h"
#include "user.h"

int main(int argc, char *argv[]) 
{
  if(argc != 3) 
  {
    printf(2, "Invalid arguments\n");
    exit();
  }
  int pid = atoi(argv[1]), priority = atoi(argv[2]);
  if(changepriority(pid, priority) == -1)
    printf(2, "Invalid process ID\n");
  exit();
}
#endif