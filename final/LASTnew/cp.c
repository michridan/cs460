#include "ucode.c"

int main(int argc, char *argv[])
{
    char line[1024];
    int fd1, fd2, n;

    if(argc < 3)
        exit(-1);

    fd1 = open(argv[1], O_RDONLY);
    fd2 = open(argv[2], O_WRONLY | O_CREAT);

    if(fd1 < 0 || fd2 < 0)
    {
        print2f("Couldn't open file");
        exit(-1);
    }

    while(n = read(fd1, line, 1024))
    {
        write(fd2, line, n);
    }
}