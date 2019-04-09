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
    printf("My name's process #%d, and I'm in Umode (u4) at %x. \n", getpid(), getPA());
    printf("If you have any real questions, you should ask my parent, #%d\n", getppid());
    printf("Anyways, here's a menu.\n");
    umenu();
    printf("Is there anything I can do? : ");
    ugetline(line); 
    uprintf("\n"); 
    udo(line);
  }
}

main()
{
  ubody("four");
}
