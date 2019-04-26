#include "ucode.c"

int console, s1, s0;

int parent()
{
    int pid, status;

    while(1)
    {
        pid = wait(&status);

        if(pid == console)
        {
            printf("console died, forking new login process\n", pid);
            console = fork();
            
            if(console)
                continue;
            else
                exec("login /dev/tty0");
        }
        if(pid == s0)
        {
            printf("serial port 0 died, forking new login process\n", pid);
            s0 = fork();
            
            if(s0)
                continue;
            else
                exec("login /dev/ttyS0");
        }
        if(pid == s1)
        {
            printf("serial port 1 died, forking new login process\n", pid);
            s1 = fork();
            
            if(s1)
                continue;
            else
                exec("login /dev/ttyS1");
        }
        printf("child %d died with exit status = %d", pid, status);
    }
}

int main(int argc, char *argv[])
{
    int in, out;
    in = open("/dev/tty0", O_RDONLY);
    out = open("/dev/tty0", O_WRONLY);
    printf("Forking login on console\n");
    console = fork();
    if(!console)
        exec("login /dev/tty0");

    printf("Forking login on serial port 0\n");
    s0 = fork();
    if(!s0)
        exec("login /dev/ttyS0");

    printf("Forking login on serial port 1\n");
    s1 = fork();
    if(!s1)
        exec("login /dev/ttyS1");

   parent(); 
}