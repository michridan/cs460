#include "ucode.c"

int main(int argc, char *argv[])
{
    char line[1024], c;
    int fd, n = 0;

    if(argc < 2)
        exit(-1);
    else if(!strcmp(argv[1], "_pipe_"))
        fd = 0;
    else
        fd = open(argv[1], O_RDONLY);

    if(fd < 0)
    {
        print2f("Couldn't open file");
        exit(-1);
    }

    while(fgetline(fd, line))
    {
        if(n < 20)
        {
            prints(line);
            n++;
        }
        else
        {
            c = getc();
            if(c == ' ')
                n = 0;
            else if(c == '\n' || c == '\r')
                n--;
        }
    }
}