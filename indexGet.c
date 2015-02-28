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
#define KB 1024
#define MB 1048576
#define GB 1073741824
char pathToDirectory[100];

struct DIR {
           long           d_ino;
           off_t          d_off;
           unsigned short d_reclen;
           char           d_name[];
};

void datetime(int date){
	int seconds, seconds_left,minutes,minutes_left,hours,hours_left,days,days_left,years;
	seconds = date;
	seconds = seconds - 2*31536000;
	seconds_left = seconds%60;
	minutes= seconds/60;
	minutes_left = minutes%60;
	hours = minutes/60;
	hours_left = hours%24;
	days = hours/24;
	years = days/365.25;
	days_left = days - years*365.25;
	years = 1972 + years;
	hours_left = hours_left + 5;
	minutes_left = minutes_left + 30;
	if(minutes_left >= 60)
	{
		minutes_left = minutes_left - 60;
		hours_left = hours_left + 1;
	}
	if(hours_left >= 24)
	{
		hours_left = hours_left -1;
		years = years + 1;
	}
	days_left = days_left + 2;
	int ndates[] = {1,32,60,91,121,152,183,213,244,274,305,335,366};
	int ldates[] = {1,32,60,92,122,153,184,214,245,276,306,336,367};
	int i = 0;
	if((years-1972)%4==0)
	{
		while(1)
		{
			if(days_left < ldates[i+1])
				break;
			if(days_left == ldates[i+1] - 1)
			{
				i++;
				break;
			}
			i++;
		}
	}
	else
	{	
		while(1)
		{
			if(days_left <= ndates[i+1])
				break;
			if(days_left == ndates[i+1] - 1)
			{
				i++;
				break;
			}
			i++;
		}
	}
	i++;
	int daym = 0, month = 0;
	month = i;
	i = i-1;
	daym = days_left - ndates[i];
	struct months{
		char name[4];
	};
	struct months k[] = {"Jan","Feb","Mar","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
	// if(daym < 10)
	// 	printf("0");
	printf("%d ",daym);
	if(i > 2)
		printf("%s ",k[i-2].name);
	else
		printf("%s ",k[i].name );
	printf("%d ",years );
	printf("%d:",hours_left);
	if(minutes_left < 10)
		printf("0");
	printf("%d:",minutes_left);
	printf("%02d ",seconds_left );
}

int dateToEpoch(){
	printf("An example of date format is: 27 Feb 2015 18:20:28\n");
	struct tm tm;
	time_t t;
	char date[21];
	int i = 0;
	while(i< 21)
	{
		date[i] = getchar();
		i++;
	}
	date[20] = NULL;
	date[21] = NULL;
	date[22] = NULL;

	if (strptime(date, "%d %b %Y %H:%M:%S", &tm) == NULL)
	{
	    printf("You didn't follow the example, sorry.\n");
		return 0;
	}
	tm.tm_isdst = -1;      /* Not set by strptime(); tells mktime()
	                          to determine whether daylight saving time
	                          is in effect */
	t = mktime(&tm);
	if (t == -1)
	    /* Handle error */;
	return t;
}

void human(int bsize){
	float hsize;
	if(bsize > GB)
	{
		hsize = ((float)bsize)/GB;
		printf("%.1fG ",hsize );
	}
	else if(bsize > MB)
	{
		hsize = ((float)bsize)/MB;
		printf("%.1fM ",hsize);
	}
	else if(bsize > KB)
	{
		hsize = ((float)bsize)/KB;
		printf("%.1fK ",hsize);
	}
	else if(bsize > 0)
	{
		hsize = bsize;
		printf(" %d ",bsize);
	}
	else
	{
		hsize = 0;
		printf("%4d ",hsize);
	}
}

int getMoreInfo(char* path,char* name, int start_time, int end_time, int longlist, int regexFlag, regex_t regex)
{
	char temppath[PATH];
	
    strcpy(temppath,path);strcat(temppath, "/");strcat(temppath,name);
	struct stat fileStat;
	int k;
    if((k = lstat(temppath,&fileStat)) < 0)
    {    
    	printf("Error has occured\n");
	    return 1;
	}
	if(fileStat.st_mtime <= end_time && fileStat.st_mtime >= start_time || longlist == 1)
	{
		if(regexFlag)
		{
			int reti;
			char msgbuf[100];
			reti = regexec(&regex, name, 0, NULL, 0);
			if( !reti );
			else if( reti == REG_NOMATCH ){
					printf("--\n");
			        return 0;
			}
			else{
			        regerror(reti, &regex, msgbuf, sizeof(msgbuf));
			        fprintf(stderr, "Regex match failed: %s\n", msgbuf);
			        exit(1);
			}
		}
		datetime(fileStat.st_mtime);
		int dflag,lflag;
		dflag = (S_ISDIR(fileStat.st_mode));
		lflag = (S_ISLNK(fileStat.st_mode));
		if(dflag)
			printf("D ");
		else if(lflag)
			printf("L ");
		else
			printf("R ");
		human(fileStat.st_size);
		printf("%s ",name);
		printf("\n");
	}
	else
		printf("-\n");
	return 0;
}

void getListOfFiles(char* string, int level, int start_time, int end_time, int longlist, int regexFlag, regex_t regex)
{
		char pathToDirectory[100];
		int fine,exists;
		char buf[PATH];
	 	int fd,canread,offset;
	    struct DIR *d;
	    char d_type;
	    strcpy(pathToDirectory,string);
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
            	for (i = 0; i < level; ++i)
            	{
            		printf("\t");
            	}
            	// printf("%-10s ",(d_type == DT_REG) ?  "File" :
             //                    (d_type == DT_DIR) ?  "Directory" :
             //                    (d_type == DT_FIFO) ? "FIFO" :
             //                    (d_type == DT_SOCK) ? "socket" :
             //                    (d_type == DT_LNK) ?  "symlink" :
             //                    (d_type == DT_BLK) ?  "block dev" :
             //                    (d_type == DT_CHR) ?  "char dev" : "???");
            	//printf("%s\n", d->d_name);
            	//printf("%s/%s\n",string,d->d_name );
            	getMoreInfo(string,d->d_name,start_time,end_time,longlist,regexFlag,regex);
        		if((d_type == DT_DIR) && d->d_name[0] != '.')
        		{	
        			char dup[100];
        			strcpy(dup,string);
        			strcat(dup, "/");
        			strcat(dup,d->d_name);
        			getListOfFiles(dup,level + 1,start_time,end_time,longlist,regexFlag,regex);
        		}
            	offset += d->d_reclen;
	        }
	    }
}


void historyOfRequests(char* pathToDirectory, int start_time, int end_time, int longlist, int regexFlag, char* regex)
{
	FILE * historyOfRequests;
	historyOfRequests = fopen ("historyOfRequests.txt", "a");
	fprintf(historyOfRequests,"%s %d %d %d %d %s\n",pathToDirectory,start_time,end_time,longlist,regexFlag,regex);
	fclose(historyOfRequests);
	return 0;		
}

int main(int argc, char const *argv[])
{
	// char* pathToDirectory = malloc(sizeof(char)*100);
	char string[100];
	char regex_input[100];
	regex_input[1] = '\0';
	regex_input[0] = 'n';
	printf("Path to Directory Input: ");
	scanf("%s",string);
	realpath(string,pathToDirectory);
	int start_time = 0,end_time = 0,longlist, regexFlag;
	regex_t regex;
	printf("LongList Flag Input: ");
	scanf("%d",&longlist);
	printf("RegexFlag Input: ");
	scanf("%d",&regexFlag);
	if(longlist == 0)
	{
		getchar();
		start_time = dateToEpoch();
		end_time = dateToEpoch();
		if(regexFlag == 0)
			getListOfFiles(pathToDirectory,0,start_time,end_time,longlist,regexFlag,regex);
		else if(regexFlag == 1)
		{	
			int reti;
			scanf("%s",regex_input);
			reti = regcomp(&regex,regex_input, 0);
			if(reti)
			{
				fprintf(stderr, "Could not compile regex\n");
				exit(1);
			}
			getListOfFiles(pathToDirectory,0,start_time,end_time,longlist,regexFlag,regex);
			regfree(&regex);
		}
	}
	else if (longlist == 1)
	{
		if(regexFlag == 0)
			getListOfFiles(pathToDirectory,0,start_time,end_time,longlist,regexFlag,regex);
		else if(regexFlag == 1)
		{
			int reti;
			scanf("%s",regex_input);
			reti = regcomp(&regex,regex_input, 0);
			if(reti)
			{
				fprintf(stderr, "Could not compile regex\n");
				exit(1);
			}
			getListOfFiles(pathToDirectory,0,start_time,end_time,longlist,regexFlag,regex);
			regfree(&regex);
		}
	}
	printf("%s\n",regex_input );
	historyOfRequests(pathToDirectory,start_time,end_time,longlist,regexFlag,regex_input);
	
	return 0;
}