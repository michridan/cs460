#include "ucode.c"

char bins[20][128];
char line[64];
char cwd[128];
char HOME[128];

void cd(char *path);
int newProcess(char *line, char *env[]);

int main(int argc, char *argv[], char *env[])
{
	char *myargv[64], temp[128], cmd[128], args[128], *ptemp;
	int i = 0, j = 0, pid, status;

	while(1)
	{
		//	Prompt for input
		getcwd(cwd);
		printf("MySH @ %s: ", cwd);
		gets(line);

		//	Parse Command
		//	Handle "cd" and "exit"
		if(!strncmp(line, "cd", 2))
		{
			chdir(&line[3]);
		}
		else if(!strncmp(line, "exit", 4))
			exit(0);
		else
		{
			//	Send it off
			strcpy(temp, "proc ");
			strcat(temp, line);
			pid = fork();

			if(pid)
			{
				wait(&status);
			}
			else
			{
				exec(temp);
			}
		}
	}
	return 0;
}
