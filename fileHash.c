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

struct DIR {
           long           d_ino;
           off_t          d_off;
           unsigned short d_reclen;
           char           d_name[];
};

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
        printf ("%s can't be opened.\n", file);
        return 0;
    }

    MD5_Init (&mdContext);
    while ((bytes = fread (data, 1, 1024, inFile)) != 0)
        MD5_Update (&mdContext, data, bytes);
    MD5_Final (c,&mdContext);
    fclose (inFile);
    return c;
}

// Filehash outputs to a file named fileHashOutput
int fileHash(int verifyFlag, char* name, char* pathOfSharedFolder)
{
	FILE *fileHashOutput;
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


int main(int argc, char const *argv[])
{
	char string[100];
	char pathToDirectory[100];
	char name[100];
	bzero(name,sizeof(name));
	scanf("%s",string);
	realpath(string,pathToDirectory);
	int verifyFlag = 0;
	scanf("%d",&verifyFlag);
	if(verifyFlag)
		scanf("%s",string);
	fileHash(verifyFlag,name,pathToDirectory);
	return 0;
}