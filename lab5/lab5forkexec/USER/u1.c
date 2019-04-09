#include "ucode.c"

int ubody(char *name)
{
  int pid, ppid;
  char line[64];
  u32 mode,  *up;

  mode = getcsr();
  mode = mode & 0x1F;
  printf("CPU mode=%x\n", mode);
  pid = getpid();
  ppid = getppid();

  while(1){
    printf("This is process #%d in Umode (u1) at %x parent=%d\n", getpid(), getPA(), getppid());
    umenu();
    printf("input a command : ");
    ugetline(line); 
    uprintf("\n"); 
    udo(line);
  }
}

int main()
{
  ubody("one");
}
 
