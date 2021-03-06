#include <stdio.h>
#include <openssl/md5.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/syscall.h>
#include <time.h>
#include <regex.h>

#define PATH 100000


// -lssl -lcrypto required to compile
unsigned char* findChecksum(char* file)
{
	unsigned char c[MD5_DIGEST_LENGTH];
    int i;
    FILE *inFile = fopen (file, "rb");
    MD5_CTX mdContext;
    int bytes;
    unsigned char data[1024];

    if (inFile == NULL) {
        // printf ("%s can't be opened.\n", file);
        return 0;
    }

    MD5_Init (&mdContext);
    while ((bytes = fread (data, 1, 1024, inFile)) != 0)
        MD5_Update (&mdContext, data, bytes);
    MD5_Final (c,&mdContext);
    fclose (inFile);
    return c;
}

char* getFileInfo(char* name)
{
	char fileHashOutput[2000];
	char temppath[PATH];
    realpath(name, temppath);
    // printf("::%s %s::\n", name, temppath);
	struct stat fileStat;
	int k;
    if((k = lstat(temppath,&fileStat)) < 0)
    {    
    	// printf("Error has occured opening: %s\n",temppath);
	    return "Error, excuse us.";
	}
	unsigned char* checksum;
	checksum = findChecksum(temppath);
	// printf("%s\n",temppath );
	// printf("fileStat %d %d\n",fileStat.st_size,fileStat.st_mtime);
	sprintf(fileHashOutput," %s %d %d ",temppath,fileStat.st_size,fileStat.st_mtime);
	//printf("fileHashOutput: %s\n",fileHashOutput );
	int i;
	for(i = 0; i < MD5_DIGEST_LENGTH; i++)
		sprintf(fileHashOutput + strlen(fileHashOutput),"%02x", checksum[i]);
	sprintf(fileHashOutput + strlen(fileHashOutput), "\n" );
	// printf("fileHashOutput %s\n",fileHashOutput);
	return fileHashOutput;

}

// Filehash outputs to a file named fileHashOutput
int fileHash(int verifyFlag, char* name, char* pathOfSharedFolder)
{
	FILE *fileHashOutput;
	remove("fileHashOutput.txt");
	fileHashOutput = fopen ("fileHashOutput.txt", "a");
	if(verifyFlag == 1)
	{
			char temppath[PATH];
		    strcpy(temppath,pathOfSharedFolder);strcat(temppath, "/");strcat(temppath,name);
			struct stat fileStat;
			int k;
		    if((k = lstat(temppath,&fileStat)) < 0)
		    {    
		    	printf("Error has occured opening: %s\n",temppath);
			    return -1;
			}
			unsigned char* checksum;
			checksum = findChecksum(temppath);
			fprintf(fileHashOutput,"%s %d ",temppath,fileStat.st_mtime);
			int i;
			for(i = 0; i < MD5_DIGEST_LENGTH; i++)
    			fprintf(fileHashOutput,"%02x", checksum[i]);
    		fprintf(fileHashOutput, "\n" );
			fclose(fileHashOutput);
			return 0;
	}
		char pathToDirectory[100];
		int exists;
		char buf[PATH];
	 	int fd,canread,offset;
	    struct DIR *d;
	    char d_type;
	    strcpy(pathToDirectory,pathOfSharedFolder);
	    if ((fd=open(pathToDirectory,O_RDONLY)) == -1)
	    	printf("Path cannot be opened");
	    while(1) 
	    {
	        if((canread = syscall(SYS_getdents, fd, buf, PATH))== -1)
	        {
	        	if((exists = access(pathToDirectory,F_OK) ) ==  0)
	        	{
	        		printf("Something\n");
	        		break;
	        	}
	            printf("Error: Something has gone terribly wrong. Run for your life! ***BOOMMM***\n");
	            break;
	        }
	        else if (canread == 0)
	            break;
	        for (offset = 0; offset < canread;)
	        {
	        	d = (struct linux_dirent *) (buf + offset);
            	d_type = *(buf + offset + d->d_reclen - 1);
            	int i;
            	if(d_type == DT_REG)
            		fileHash(1,d->d_name,pathToDirectory);
        		if((d_type == DT_DIR) && d->d_name[0] != '.')
        		{	
        			char dup[100];
        			strcpy(dup,pathToDirectory);
        			strcat(dup, "/");
        			strcat(dup,d->d_name);
        			fileHash(0,"\0",dup);
        		}
            	offset += d->d_reclen;
	        }
	    }
}

