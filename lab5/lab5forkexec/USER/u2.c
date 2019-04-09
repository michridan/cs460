#include "ucode.c"

int ubody(char *name)
{
  int pid, ppid;
  char line[64];
  u32 mode,  *up;

  mode = getcsr();
  mode = mode & 0x1F;
  printf("CPU MODE=%x\n", mode);
  pid = getpid();
  ppid = getppid();

  while(1){
    printf("THIS IS PROCESS #%d IN UMODE (U2) AT %x PARENT=%d\n", getpid(), getPA(), getppid());
    umenu();
    printf("INPUT A COMMAND : ");
    ugetline(line); 
    uprintf("\n"); 
    udo(line);
  }
}

int main()
{
  ubody("two");
}
