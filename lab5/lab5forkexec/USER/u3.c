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
    printf("Avast! Thar be process #%d in Umode (u3) at %x pirate=%d\n", getpid(), getPA(), getppid());
    umenu();
    printf("walk the plank : ");
    ugetline(line); 
    uprintf("\n"); 
    udo(line);
  }
}

main()
{
  ubody("three");
}
