#include "ucode.c"
//#include "uio.c"

int in, out, err;
char username[128], password[128];

int main(int argc, char *argv[])
{
    int key, uid, gid, i = 0, j;
    char home[128], name[128], user_check[128], pass_check[128], line[256], cmd[128], temp[128], program[128];
    close(0);
    close(1);
    in = open(argv[1], O_RDONLY);
    out = open(argv[1], O_WRONLY);
    err = open(argv[1], O_WRONLY);
    fixtty(argv[1]);

    key = open("/etc/passwd", O_RDONLY);

    while(1)
    {
        printf("username:");
        gets(username);
        printf("password:");
        gets(password);


        while(fgetline(key, line))
        {
            i = 0;
            for(j = 0; line[i] != ':'; i++, j++)
            {
                user_check[j] = line[i];
            }
            i++;
            user_check[j] = '\0';
            
            for(j = 0; line[i] != ':'; i++, j++)
            {
                pass_check[j] = line[i];
            }
            i++;
            pass_check[j] = 0;

            if(strcasecmp(user_check, username) || strcmp(pass_check, password))
                continue;
            
            for(j = 0; line[i] != ':'; i++, j++)
            {
                temp[j] = line[i];
            }
            i++;
            temp[j] = 0;
            gid = atoi(temp);

            for(j = 0; line[i] != ':'; i++, j++)
            {
                temp[j] = line[i];
            }
            i++;
            temp[j] = 0;
            uid = atoi(temp);

            for(j = 0; line[i] != ':'; i++, j++)
            {
                name[j] = line[i];
            }
            i++;
            name[j] = 0;

            for(j = 0; line[i] != ':'; i++, j++)
            {
                home[j] = line[i];
            }
            i++;
            home[j] = 0;

            for(j = 0; line[i] != '\n' && line[i] != 0; i++, j++)
            {
                program[j] = line[i];
            }
            program[j] = 0;

            printf("Welcome %s, change uid to %d and home to %s\n", name, uid, home);
            chuid(uid, gid);
            chdir(home);
            exec(program);
        }
        lseek(key, 0, 0);
    }
}