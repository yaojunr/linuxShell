#ifndef _afunction_h
#define _afunction_h


void forkFunction(char* buf)
{
	pid_t child_pid;
	int status;
	char *prog = NULL;
	child_pid = fork();
	buf = strtok(buf,"\n");
	if (child_pid != 0) 
	{
		printf("It is processing,please wait. \n");
		waitpid(-1, &status, 0);
		printf("The mission complete. \n");
	} 
	else
	{
		if (strchr(buf,' '))   // e.g gedit xxx
		{
			prog = strtok(buf," ");
			char *argv[] = {prog,(char *)0};
			printf("This is the child process, with id %d\n", (int) getpid ());
			execv("hello.out",argv);
			exit(0);
		}
		else 	// e.g ./a.out
		{
			char *tmp;
			char name[60],path[60];
			memset(name, '\0', sizeof(name));
			memset(path, '\0', sizeof(path));
			char *filepath = path;
			char *filename = name;
			int num,i;

			char dir[80];
			memset(dir, '\0', sizeof(dir));
			getcwd(dir,sizeof(dir));

			if(tmp = strrchr(buf,'/'))
			{
				for (i=0;i < tmp - buf;i++)	// music/a.out			
					filepath[i] = buf[i];

				if (strstr(filepath,"./") == filepath)   // ./xxx
					filepath += 2;
				i++;
				num = 0;
				for (;i<strlen(buf);i++)
				{
					filename[num] = buf[i];
					num++;
				}
			}
			char *argv[] = {(char *)0};
			if (!chdir(filepath))
			{
				if(execv(filename,argv) == -1)
					printf("No such file.\n");
				exit(0);
				
			}
			else printf("No such directory.\n");
			chdir(dir);
			exit(0);
		}

	}
}

void pwdFunction(char *buf,int redirect)
{
	char dir[80];
	char *originPath;

	char filename[80];
	char *filepath = filename;
	memset(dir, '\0', sizeof(dir));
	memset(filename, '\0', sizeof(filename));
	getcwd(dir,sizeof(dir));
	if (redirect)
	{
		FILE *fp;
		int i;
		int num=0;
		strtok(buf,">");
		originPath = strtok(NULL,">");
		for (i=0;i<strlen(originPath)-1;i++)
			if (originPath[i] == ' ')
				continue;
			else 
			{
				filepath[num] = originPath[i];
				num++;
			}	
		fp = fopen(filepath,"w");
		fprintf(fp, "%s\n", dir);
		fclose(fp);

	}
	else
		printf("%s\n", dir);

}

void cdFunction(char *buf,char **pathName,char *oriDir)  //want to change the pointer of pathname
{
	char *workDir;
	char *tmp;	

	if (strcmp(buf,"cd\n" ) == 0)
	{
		chdir(oriDir);
		*pathName = "~";
	}
	else 
	{
		strtok(buf," ");
		tmp = strtok(NULL," ");
		workDir = strtok(tmp,"\n");
		if (!strcmp(workDir,"~"))   //cd ~
		{
			chdir(oriDir);
			*pathName = "~";
			return;
		}

		if (strstr(workDir,"./") == workDir)   // cd ./xxx
			workDir += 2;
		if (strchr(workDir,'~') == workDir)		// cd ~/xxx
		{
			workDir++;
			char tmpChar[80];
			strcpy(tmpChar,oriDir);
			strcat(tmpChar,workDir);
			workDir = tmpChar;
		}

		if (!chdir(workDir))              // cd xxx
		{	
			char dir[80];
			static char path[80];
			memset(dir, '\0', sizeof(dir));
			memset(path, '\0', sizeof(path));		
			getcwd(dir,sizeof(dir));

			char *data = dir;
			tmp = path;
			int i=0;

			if (strlen(dir)>strlen(oriDir))
			{	
				strcpy(path,"~");
				for (i=0;i<strlen(dir)-strlen(oriDir);i++)
					tmp[i+1] = data[i+strlen(oriDir)];		//output as ~/xxx	
				//strcat can't join the pointer but the string; it needs space

			}
			else
				strcpy(path,dir); //output as /home/xxx
			*pathName = path;

		}
		else
			printf("No such directory.\n");
	}
}

void judgeMode(int mode,char *str)
{
	strcpy(str,"----------");
	if (S_ISDIR(mode)) str[0] = 'd';
	if (S_ISCHR(mode)) str[0] = 'c';
	if (S_ISBLK(mode)) str[0] = 'b';
	if (S_ISLNK(mode)) str[0] = 'l';
	if (S_ISFIFO(mode)) str[0] = 'p';

	if(mode & S_IRUSR) str[1] = 'r';   //mask
    if(mode & S_IWUSR) str[2] = 'w';
	if(mode & S_IXUSR) str[3] = 'x';

	if(mode & S_IRGRP) str[4] = 'r';
	if(mode & S_IWGRP) str[5] = 'w';
	if(mode & S_IXGRP) str[6] = 'x';
	
	if(mode & S_IROTH) str[7] = 'r';
	if(mode & S_IWOTH) str[8] = 'w';
	if(mode & S_IXOTH) str[9] = 'x';
}

char* getUid(uid_t uid)
{
	struct passwd *user;
	user = getpwuid(uid);
	return user->pw_name;
}

char* getGid(gid_t gid)
{
	struct group *data;
	data = getgrgid(gid);
	return data->gr_name;
}

void lsFunction(char *buf,int redirect)  //*buf is a pointer to the string buf
{
	struct dirent **namelist;
	int i,total,num;
	char dir[80];
	memset(dir, '\0', sizeof(dir));
	char *name,*p,*tmp;
	FILE *fp;
	char *filepath[10];     //store the filepath
	for (i=0;i<10;i++) filepath[i] = NULL;   //initialize the array
	getcwd(dir,sizeof(dir));    //default filepath
	
	if (redirect)
	{
		char *originPath;
		char storageFile[80];
		char *storagePath = storageFile;
		memset(storageFile, '\0', sizeof(storageFile));
		num = 0;
		tmp = strtok(buf,">");
		originPath = strtok(NULL,">");
		for (i=0;i<strlen(originPath)-1;i++)
			if (originPath[i] == ' ')
				continue;
			else 
			{
				storagePath[num] = originPath[i];
				num++;
			}	
		fp = fopen(storagePath,"w");
	}

	if (redirect) buf = tmp;
	int all=0,inode=0,detail=0;
	p = strtok(buf, "\n");
	strtok(p," ");
	i=0;      //record the num of path
	while(p = strtok(NULL," "))
	{
		if (strchr(p,'-') == p)   // judge the parameter
		{
			if (strchr(p,'a'))
				all++;
			if (strchr(p,'i'))
				inode++;
			if (strchr(p,'l'))
				detail++;
			if (all+inode+detail==0)
			{
				printf("No such command.\n");   //don't support other command 
				return;
			}
		}


		else
		{	
			filepath[i] = p;
			
			if (strstr(filepath[i],"./") == filepath[i])   // ls ./xxx
				filepath[i] += 2;
			
			i++;

		}
	}

	int pathNum = i;
	if (pathNum == 0) pathNum++;
	int j;

	if (!redirect)
	for (j=0;j<pathNum;j++)
	{
		
		if (filepath[j])
		if (!chdir(filepath[j])) 	{if (pathNum>1) printf("%s:\n", filepath[j]);}		// change directory if necessary
		else 
		{
			printf("ls: cannot access %s: No such directory.\n",filepath[j]);
			continue;
		}
		total = scandir(".",&namelist,0,alphasort);
		int num = 0,col = 4;
		struct stat info;
		for (i=0;i<total;i++)
		{
			name = namelist[i]->d_name;
			stat(name,&info);	
			if (!all)
			{
				p = strchr(name,'.');
				if (p == name) continue;
			}
			num++;
			if (inode)
			{
				printf("%d ", (int)info.st_ino);
				col = 3;
			}
			if (detail)
			{
				char modeStr[11];
				memset(modeStr, '\0', sizeof(modeStr));
				judgeMode(info.st_mode,modeStr);
				printf("%s  ", modeStr);
				printf("%2d ", (int)info.st_nlink);
				printf("%s ", getUid(info.st_uid));
				printf("%s ", getGid(info.st_gid));
				printf("%6ld ", (long)info.st_size);
				printf("%.12s ",4+ctime(&info.st_mtime));
				printf("%-17s\n", name);
			}
			else
			{
				printf("%-17s", namelist[i]->d_name);
				if (num%col == 0)
					printf("\n");
			}
		}
		if (!detail && num%col != 0)
			printf("\n");
		chdir(dir);        //relocate to the original path
	}

	else             //output redirectory
	for (j=0;j<pathNum;j++)
	{
		
		if (filepath[j])
		if (!chdir(filepath[j])) 	{if (pathNum>1) fprintf(fp,"%s:\n", filepath[j]);}		// change directory if necessary
		else 
		{
			fprintf(fp,"ls: cannot access %s: No such directory.\n",filepath[j]);
			continue;
		}
		total = scandir(".",&namelist,0,alphasort);
		int num = 0,col = 4;
		struct stat info;
		for (i=0;i<total;i++)
		{
			name = namelist[i]->d_name;
			stat(name,&info);	
			if (!all)
			{
				p = strchr(name,'.');
				if (p == name) continue;
			}
			num++;
			if (inode)
			{
				fprintf(fp,"%d ", (int)info.st_ino);
				col = 3;
			}
			if (detail)
			{
				char modeStr[11];
				memset(modeStr, '\0', sizeof(modeStr));
				judgeMode(info.st_mode,modeStr);
				fprintf(fp,"%s  ", modeStr);
				fprintf(fp,"%d ", (int)info.st_nlink);
				fprintf(fp,"%s ", getUid(info.st_uid));
				fprintf(fp,"%s ", getGid(info.st_gid));
				fprintf(fp,"%6ld ", (long)info.st_size);
				fprintf(fp,"%.12s ",4+ctime(&info.st_mtime));
				fprintf(fp,"%-17s\n", name);
			}
			else
			{
				fprintf(fp,"%-17s", namelist[i]->d_name);
				if (num%col == 0)
					fprintf(fp,"\n");
			}
		}
		if (!detail && num%col != 0)
			fprintf(fp,"\n");
		chdir(dir);        //relocate to the original path
	}
	fclose(fp);
}

void mkdirFunction(char *buf)
{
	char *tmp;
	int num = 0,i;
	int parent = 0;
	char name[60],path[60];
	memset(name, '\0', sizeof(name));
	memset(path, '\0', sizeof(path));
	char *originPath = NULL;
	char *filepath = path;
	char *filename = name;

	char dir[80];
	memset(dir, '\0', sizeof(dir));
	getcwd(dir,sizeof(dir));

	strtok(buf," ");
	while(tmp = strtok(NULL," "))
	{
		if (strchr(tmp,'-') == tmp)   // judge the parameter
		{
			if (strchr(tmp,'p'))
			{
				originPath = strtok(NULL," ");
				originPath = strtok(originPath,"\n");
				parent++;
			}
			if (!originPath)
			{
				printf("No such command.\n");   //don't support other command 
				return;
			}
		}
		else originPath = strtok(tmp,"\n");
	}

	if (!parent)   //without the para of -p
	{
		if(tmp = strrchr(originPath,'/'))
		{
			for (i=0;i < tmp - originPath;i++)	//1234/5			
				filepath[i] = originPath[i];

			if (strstr(filepath,"./") == filepath)   // mkdir ./xxx
					filepath += 2;
			i++;
			num = 0;
			for (;i<strlen(originPath);i++)
			{
				filename[num] = originPath[i];
				num++;
			}
		}
		else 
		{
			filepath = ".";
			filename = originPath;
		}
		if (!chdir(filepath))
			mkdir(filename,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		else printf("mkdir: cannot create directory '%s': No such file or directory.\n",originPath);
		chdir(dir);
	}

	else	//-p     xxx/xxx/xxx
	{
		tmp = strtok(originPath,"/");
		do
		{
			if(!chdir(tmp))
				continue;
			else
			{
				mkdir(tmp,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
				chdir(tmp);
			}
		} while(tmp = strtok(NULL,"/"));
		chdir(dir);
	}
}

void catFunction(char *buf)
{
	FILE *fp;
	char *tmp,*filename;
	char ch,lch;
	strtok(buf," ");
	tmp = strtok(NULL," ");
	filename = strtok(tmp,"\n");
	if (!(fp = fopen(filename,"r")))
		printf("Such file doesn't exist\n");

	while ((ch=fgetc(fp))!=EOF)
    {
        printf("%c", ch);
        lch = ch;
    }
    fclose(fp);
    if (lch != 10)
    	printf("\n");
}


#endif