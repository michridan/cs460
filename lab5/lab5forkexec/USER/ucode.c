typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned int    u32;

#include "string.c"
#include "uio.c"

int udo(char *line)
{
    if (strcmp(line, "getpid")==0)
       ugetpid();
    if (strcmp(line, "getppid")==0)
       ugetppid();
    if (strcmp(line, "ps")==0)
       ups();
    if (strcmp(line, "chname")==0)
       uchname();
    if (strcmp(line, "switch")==0)
       uswitch();
    if (strcmp(line, "kfork")==0)
      ukfork();
    if (strcmp(line, "wait")==0)
      uwait();
    if (strcmp(line, "exit")==0)
      uexit();
    if (strcmp(line, "sleep")==0)
      usleep();
    if (strcmp(line, "wakeup")==0)
      uwakeup();
    if (strcmp(line, "fork") == 0)
      ufork();
    if (strcmp(line, "exec") == 0)
      uexec();
}

int umenu()
{
  uprintf("------------------------------------------------------------------------\n");
  uprintf("|getpid getppid ps chname switch kfork wait exit sleep wakeup fork exec|\n");
  uprintf("------------------------------------------------------------------------\n");
}

int getpid()
{
  int pid;
  pid = syscall(0,0,0,0);
  return pid;
}    

int getppid()
{ 
  return syscall(1,0,0,0);
}

int ugetpid()
{
  int pid = getpid();
  uprintf("pid = %d\n", pid);
}

int ugetppid()
{
  int ppid = getppid();
  uprintf("ppid = %d\n", ppid);
}

int ups()
{
  return syscall(2,0,0,0);
}

int uchname()
{
  char s[32];
  uprintf("input a name string : ");
  ugetline(s);
  printf("\n");
  return syscall(3,s,0,0);
}

int uswitch()
{
  return syscall(4,0,0,0);
}

int ukfork()
{
  return syscall(5,0,0,0);
}

int uwait()
{
  return syscall(6,0,0,0);
}

int uexit()
{
  return syscall(7,0,0,0);
}

int usleep()
{
  printf("Enter sleep code:");
  return syscall(8,geti(),0,0);
}

int uwakeup()
{
  printf("Enter wakeup status: ");
  return syscall(9,geti(),0,0);
}

int ufork()
{
  int pid = syscall(10,0,0,0);
  return pid;
}

int uexec()
{
  char *s;
  printf("Enter the line you would like to execute.\n");
  ugetline(s);
  return syscall(11,s,0,0);
}

int ugetc()
{
  return syscall(90,0,0,0);
}

int uputc(char c)
{
  return syscall(91,c,0,0);
}

int getPA()
{
  return syscall(92,0,0,0);
}
