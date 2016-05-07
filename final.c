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


int main()
{
    
    int pipe_fd = -1;
    int res = 0;
    int open_mode = O_RDONLY;
    char buf[128];
    int redirect = 0;
    char *userName = "alex@root:";
    char *pathName = "~";

    char pipeName[80],oriDir[80];  //locate the pipe
    memset(pipeName, '\0', sizeof(pipeName));
    getcwd(pipeName,sizeof(pipeName));
    strcpy(oriDir,pipeName);
    char *pn = "/my_pipe";
    strcat(pipeName,pn);


    if(access(pipeName,F_OK)==-1)            
    {
        res=mkfifo(pipeName,0766);
        if(res!=0)
        {
            fprintf(stderr,"Could not creat fifo %s\n",pipeName);
            exit(1);
        }
    }
    printf("The pipe name is my_pipe.\n");
    memset(buf, '\0', sizeof(buf));

    printf("The origin root (as '~') is ");  //output the origin setting
    pwdFunction("pwd",0);
    printf("%s%s$\n", userName,pathName);

    pipe_fd = open(pipeName, open_mode);

    if(pipe_fd != -1)
    {
       
    	while (1)
    	{	
    		
    		if (res = read(pipe_fd, buf,128))
    		{
    			if (strcmp(buf,"exit\n")!=0)
    			{
    				//printf("%s", buf);
    				//printf("%d", strcmp(buf,"cd"));32
                    if (strchr(buf,'>'))
                        redirect++;

    				if (strcmp(buf,"cd" ) == 32 || strcmp(buf,"cd\n" ) == 0)
    					cdFunction(buf,&pathName,oriDir); //32:cd xxx; 0:cd
    				else if (strstr(buf,"pwd") == buf)
    					pwdFunction(buf,redirect);//pwd                
    				else if (strstr(buf,"ls") == buf)
    					lsFunction(buf,redirect);//ls
                    else if (strstr(buf,"mkdir") == buf)
                        mkdirFunction(buf); //mkdir
                    else if (strstr(buf,"cat") == buf)
                        catFunction(buf); //cat
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
    			pipe_fd = open(pipeName, open_mode);
    		}
    		memset(buf, '\0', sizeof(buf));
    	}
        close(pipe_fd);
    }
    else
        printf("%s\n", "fail");

}