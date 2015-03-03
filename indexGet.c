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
FILE* fp;

/*
	Format for index-me-get.txt
	{
	"file_list": [
				{"date":"2 Mar 2015", "time": "7:31:59", "type":"D", "size":"680","name":"."},
				{"date": "1 Mar 2016", "time": "25:15:10", "type": "D", "size": "442", "name": ".."},
				{"date": "2 Mar 2015", "time": "6:15:49", "type": "D", "size": "476", "name": ".git"},
				{"date": "1 Mar 2016", "time": "27:46:32", "type": "R", "size":"13.4K", "name": "a.out"},
				{"date": "21 Feb 2015", "time": "16:52:56", "type": "R", "size":"4.4K", "name": "backup.c"},
				{"date": "1 Mar 2016", "time": "26:34:07", "type": "R", "size":"26.5M", "name": "client.c"}
			]
}*/


char pathToDirectory[100];
int JSONFlag;
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
	// 	fprintf(fp,"0");
	if(JSONFlag)
		fprintf(fp,"{\"date\":\"");
	fprintf(fp,"%d ",daym);
	if(i > 2)
		fprintf(fp,"%s ",k[i-2].name);
	else
		fprintf(fp,"%s ",k[i].name );
	fprintf(fp,"%d",years );
	if(JSONFlag)
		fprintf(fp,"\",\"time\":\"");
	fprintf(fp,"%d:",hours_left);
	if(minutes_left < 10)
		fprintf(fp,"0");
	fprintf(fp,"%d:",minutes_left);
	fprintf(fp,"%02d",seconds_left );
	if(JSONFlag)
		fprintf(fp,"\",");
	else
		fprintf(fp,"");
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
		fprintf(fp,"%.1fG",hsize );
	}
	else if(bsize > MB)
	{
		hsize = ((float)bsize)/MB;
		fprintf(fp,"%.1fM",hsize);
	}
	else if(bsize > KB)
	{
		hsize = ((float)bsize)/KB;
		fprintf(fp,"%.1fK",hsize);
	}
	else if(bsize > 0)
	{
		hsize = bsize;
		fprintf(fp,"%d",bsize);
	}
	else
	{
		hsize = 0;
		fprintf(fp,"%4d",hsize);
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
    	fprintf(stderr,"Error has occured\n");
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
					fprintf(fp,"--\n");
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
			if(JSONFlag)
				fprintf(fp,"\"type\":\"D\", ");
			else
				fprintf(fp,"D");
		else if(lflag)
			if(JSONFlag)
				fprintf(fp,"\"type\":\"L\", ");
			else
				fprintf(fp,"L");
		else
			if(JSONFlag)
				fprintf(fp,"\"type\":\"R\", ");
			else
			fprintf(fp,"R");
		//"size":"680","name":"."},
		if(JSONFlag)
			fprintf(fp,"\"size\":\"");
		else
			fprintf(fp,"");
		human(fileStat.st_size);
		if(JSONFlag)
			fprintf(fp,"\",\"name\":\"");
		fprintf(fp,"%s",name);
		if(JSONFlag)
			fprintf(fp,"\"},");
		fprintf(fp,"\n");
	}
	else
		if(!JSONFlag)
			fprintf(fp,"-\n");
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
	        fprintf(fp,"Path cannot be opened");
	    while(1) 
	    {
	        if((canread = syscall(SYS_getdents, fd, buf, PATH))== -1)
	        {
	        	if((exists = access(pathToDirectory,F_OK) ) ==  0)
	        	{
	        		fprintf(fp,"Something\n");
	        		break;
	        	}
	            fprintf(fp,"Error: Something has gone terribly wrong. Run for your life! ***BOOMMM***\n");
	            break;
	        }
	        else if (canread == 0)
	            break;
	        for (offset = 0; offset < canread;)
	        {
	        	d = (struct linux_dirent *) (buf + offset);
            	d_type = *(buf + offset + d->d_reclen - 1);
            	int i;
            	// for (i = 0; i < level; ++i)
            	// {
            	// 	fprintf(fp,"");
            	// }
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

void handleIndex(int longlist, int regexFlag, char * regEX, int indexMe, int JSON)
{
	JSONFlag = JSON;
	if(!JSON)
		fp = fopen ("index.txt","w"); //ENABLE THIS
	else if(indexMe)
		fp = fopen("index-me-gui.txt","w");
	else if(!indexMe)
		fp = fopen("index-they-get.txt","w");
	if(JSON)
		fprintf(fp,"{\"file_list\": [\n");
	char *string;
	char regex_input[100];
	regex_input[1] = '\0';
	regex_input[0] = 'n';
	if (regEX[0]!='\0') {
		strcpy(regex_input, regEX);
	}
	// fprintf(fp,"Path to Directory Input: ");
	// scanf("%s",string);
	string = strdup(".");
	realpath(string,pathToDirectory);
	int start_time = 0,end_time = 0;
	regex_t regex;

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
			// fprintf(fp,"Enter Regex Input:\n");
			// scanf("%s",regex_input);
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
			// fprintf(fp,"Enter Regex Input:\n");

			// scanf("%s",regex_input);
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
	// printf("%s\n",regex_input );
	if(JSON)
		fprintf(fp,"]\n}");
	fseek(fp,-5,SEEK_END);
	fprintf(fp, " ");
	fclose(fp); //ENABLE THIS !!
	historyOfRequests(pathToDirectory,start_time,end_time,longlist,regexFlag,regex_input);
	
	return 0;
}


void periodicCheck()
{
	int childPID;
	childPID = fork();
	if(childPID !=0)
		return;
	while(1)
	{
		sleep(5);
		handleIndex(1,0,"\0",1,1);
		// handleIndex(1,0,"\0",0,1);
		// handleIndex(1,0,"\0",0,0);
	}
}