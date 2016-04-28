#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <limits.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include "afunction.h"

char userRoot = "/home/alex";


int main()
{
    const char *fifo_name = "/home/alex/my_pipe";   //pipe name should be change by user
    int pipe_fd = -1;
    int res = 0;
    int open_mode = O_RDONLY;
    char buf[128];
    int redirect = 0;
    char *userName = "alex@root:";
    char *pathName = "~";
    memset(buf, '\0', sizeof(buf));

    printf("%s%s$\n", userName,pathName);

    pipe_fd = open(fifo_name, open_mode);
    if(pipe_fd != -1)
    {
       
    	while (1)
    	{	
    		
    		if (res = read(pipe_fd, buf,128))
    		{
    			if (strcmp(buf,"exit()\n")!=0)
    			{
    				//printf("%s", buf);
    				//printf("%d", strcmp(buf,"cd"));32
                    if (strchr(buf,'>'))
                        redirect++;

    				if (strcmp(buf,"cd" ) == 32 || strcmp(buf,"cd\n" ) == 0)
    					cdFunction(buf,&pathName); //32:cd xxx; 0:cd
    				else if (strstr(buf,"pwd") == buf)
    					pwdFunction(buf,redirect);//pwd                
    				else if (strstr(buf,"ls") == buf)
    					lsFunction(buf,redirect);//ls
                    else if (strstr(buf,"mkdir") == buf)
                        mkdirFunction(buf); //mkdir
    				else if (strchr(buf,' ') || strchr(buf,'/'))   //only the command with ' ' or '/' can do fork 
    					forkFunction(buf);                         //e.g: gedit xxx ; ./a.out 
                    else
                        printf("No such command.\n");

    				printf("%s%s$\n", userName,pathName);
				}
    			else break;	
    		}		
    		else 
    		{
    			close(pipe_fd);
    			pipe_fd = open(fifo_name, open_mode);
    		}
    		memset(buf, '\0', sizeof(buf));
    	}
        close(pipe_fd);
    }
    else
        printf("%s\n", "fail");

}