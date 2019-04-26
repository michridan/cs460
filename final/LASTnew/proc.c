#include "ucode.c"

char bins[20][128];
char line[64];
char cwd[128];
char HOME[128];

//void cd(char *path);
//int newProcess(char *line, char *env[]);

int main(int argc, char *argv[])
{
	char file[64], temp[128], templ[128], tempr[64], cpy[128];
	int i, fd, redirect = 0, pid = 0, pid2 = 0, status, pd[2];

	for(i = 1; i < argc; i++)
	{
        printf("%s\n", argv[i]);
		if(!strcmp(argv[i], "|"))
		{
            strfold(templ, argv, 1, i);
            strfold(tempr, argv, i + 1, argc);
			printf("1.%s\n2.%s\n", templ, tempr);
			//	child forks another child to act as reader

            
   			pipe(pd);
            pid2 = fork();

   			if(!pid2) //	the new child needs to do the later part
   			{
   				//	redirect to write pipe
   				close(pd[0]); // WRITER MUST close pd[0]

   				dup2(pd[1], 1);   // replace 1 with pd[1]
                strcpy(temp, "proc ");
                strcat(temp, templ);

   				exec(temp);				
   				exit(100);
   			}
   			else
   			{
  				//	redirect to read pipe
   				close(pd[1]); // READER MUST close pd[1]
   				dup2(pd[0], 0);   // replace 0 with pd[0]
   				//	recursively execute the rest of the command
                strcpy(temp, "proc ");
                strcat(temp, tempr);
				strcat(temp, "_pipe_");

   				exec(temp);				
   				// Kill the child
   				exit(100);
   			}
    	}
	}

    // Look for redirects
	for(i = 1; i < argc && !redirect; i++)
	{
		if(!strcmp(argv[i], ">")) //	Write
		{
	    	fd = open(argv[i + 1], O_WRONLY | O_CREAT);
			if(fd < 0)
			{
				exit(-1);
			}
            dup2(fd, 1);
	    	redirect = 1;
			i--;
		}
		else if(!strcmp(argv[i], ">>")) //	Append
		{
			fd = open(argv[i + 1], O_WRONLY | O_APPEND | O_CREAT);
			if(fd < 0)
			{
				exit(-1);
			}
            dup2(fd, 1);
			redirect = 1;
			i--;
		}
		else if(!strcmp(argv[i], "<")) //	Read
		{
			fd = open(argv[i + 1], O_RDONLY);
			if(fd < 0)
			{
				exit(-1);
			}
            dup2(fd, 0);
			redirect = 1;
		}
	}
		
    strfold(temp, argv, 1, i);
    printf("%s\n", temp);
    exec(temp);
	//	Fork a child, let it find out what to do, get the exit code
    /*
	pid = fork();

	if(pid < 0)
		printf("ERROR: Process failed to fork");
	else
	{
		
		else //	Meaning no pipe
		{
			if(pid)
			{
				pid = wait(&status);
			}
			else
			{
				argv[0] = strtok(templ, " ");
				i = 1;

				while(argv[i] = strtok(0, " "))
				{
					i++;
				}
				argv[++i] = 0;
				//	Look for redirection
				for(i = 0; !redirect && argv[i]; i++)
				{
					if(!strcmp(argv[i], ">")) //	Write
					{
						close(1);
						open(argv[i + 1], O_WRONLY | O_CREAT);
						argv[i] = 0;
						redirect = 1;
					}
					else if(!strcmp(argv[i], "<")) //	Read
					{
						close(0);
						open(argv[i + 1], O_RDONLY);
						argv[i] = 0;
						redirect = 1;
					}
					else if(!strcmp(argv[i], ">>")) //	Append
					{
						close(1);
						open(argv[i + 1], O_WRONLY | O_APPEND | O_CREAT);
						argv[i] = 0;
						redirect = 1;
					}
				}
		
				print2f("Searching for command\n");
				for(i = 0; strcmp(bins[i], ""); i++)
				{
					strcpy(temp, bins[i]);
					strcat(temp, "/");
					strcat(temp, argv[0]);

					print2f("\ttesting\n");
					exec(temp);				
				}
				print2f("command not found\n");
				exit(100);
			}
		}
	}
    */
}

/*int newProcess(char *line, char *env[])
{
                   // Look for redirects
				for(j = 1; j < i && !redirect; j++)
				{
					if(!strcmp(argv[j], ">")) //	Write
					{
					    i++;
					    sscanf(templ, "%s %s", file, templ);
				    	close(1);
				    	open(file, O_WRONLY | O_CREAT);
				    	redirect = 1;
					}
					else if(argv[i] == ">>") //	Append
					{
						close(1);
						open(file, O_WRONLY | O_APPEND | O_CREAT);
						redirect = 1;
					}
					else if(templ[i] == "<") //	Read
					{
						sscanf(templ, "%s %s", file, templ);
						close(0);
						open(file, O_RDONLY);
						argv[i] = 0;
						redirect = 1;
					}
				}
		
				print2f("Searching for command\n");
}
            pid = fork();
			//	child forks another child to act as reader

            if(pid)
            {
                pid = wait(&status);
                exit(100);
            }
            else
            {
            
    			pipe(pd);
                pid2 = fork();

    			if(!pid2) //	the new child needs to do the later part
    			{
    				//	redirect to write pipe
    				close(pd[0]); // WRITER MUST close pd[0]
                    printf("lhs close\n");

    				dup2(pd[1], 1);   // replace 1 with pd[1]
                    printf("lhs dup");
                    strcpy(temp, "proc ");
                    strcat(temp, templ);
                    printf("%s\n", temp);

    				exec(temp);				
    				exit(100);
    			}
    			else
    			{
    				//	redirect to read pipe
    				close(pd[1]); // READER MUST close pd[1]
                    printf("rhs close\n");
    				dup2(pd[0], 0);   // replace 0 with pd[0]
                    printf("rhs dup\n");
    				//	recursively execute the rest of the command
                    strcpy(temp, "proc ");
                    strcat(temp, tempr);
                    printf("%s\n", temp);

    				exec(temp);				
    				// Kill the child
    				exit(100);
    			}
            }
*/
