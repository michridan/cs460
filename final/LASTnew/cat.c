#include "ucode.c"

int main(int argc, char *argv[])
{
    char line[1024];
    int fd;

    if(argc < 2)
    {
        while(gets(line))
        {
            printf("%s\n\r", line);
        }
    }
    else if(!strcmp(argv[1], "<") || !strcmp(argv[1], "_pipe_"))
    {
        while(getline(line))
        {
            prints(line);
        }

    }
    else
    {
        fd = open(argv[1], O_RDONLY);
        if(fd < 0)
        {
            print2f("Couldn't open file");
            exit(-1);
        }

        while(read(fd, line, 1024))
            prints(line);
    }
    
}