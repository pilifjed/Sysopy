#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <memory.h>
#include <stdlib.h>
#include <time.h>

int sc(char* s,char* s2){
    while (*s && *s2){
        if(*s == *s2){
            s++, s2++;
        }
        else
            return 0;
    }
    return 1;
}

void parsePerm(int perm){
    char res[]="rwxrwxrwx";
    for(int i=0;i<9;i++){
        if(!(perm & (1<<i))){
            res[8-i]='-';
        }
    }
    printf("%s\t",res);
}

void parseTime(time_t t){
    struct tm *tm;
    char buf[200];
    tm = localtime(&t);
    strftime(buf, sizeof(buf), "%d.%m.%Y %H:%M:%S", tm);
    printf("%s\n", buf);

}

int cmpTime(time_t t1,time_t t2,char mode){
    switch(mode){
        case '>':
            return difftime(t1, t2) > 0;
        case '<':
            return difftime(t1, t2) < 0;
        default:
            return difftime(t1, t2) == 0;
    }
}

void getFiles(char * cwd, char mode, time_t usrTime){
    DIR *dir = opendir(cwd);
    struct dirent *dirPtr;
    struct stat dirDet;

    while ((dirPtr = readdir(dir))) {
        if(!sc(dirPtr->d_name,".") && !sc(dirPtr->d_name,"..")){
            char d[1024] = "";
            strcat(d,cwd);
            strcat(d,"/");
            strcat(d,dirPtr->d_name);
            stat(d, &dirDet);
            if(S_ISDIR(dirDet.st_mode) && !S_ISLNK(dirDet.st_mode)){
                getFiles(d,mode,usrTime);
            } else if(S_ISREG(dirDet.st_mode)){
                if(cmpTime(dirDet.st_mtime,usrTime,mode)){
                    printf("%s\t", d);
                    printf("%lldB\t", dirDet.st_size);
                    parsePerm(dirDet.st_mode & 07777);
                    parseTime(dirDet.st_mtime);
                }

            }
        }
    }
}
time_t parseUsrTime(int s, int m, int h, int d, int mn, int y){
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    timeinfo->tm_year = y - 1900;
    timeinfo->tm_mon = mn - 1;
    timeinfo->tm_mday = d;
    timeinfo->tm_hour = h;
    timeinfo->tm_min = m;
    timeinfo->tm_sec = s;
    return mktime ( timeinfo );
}

int a2i(char *s)
{
    int sign=1;
    if(*s == '-') {
        sign = -1;
        s++;
    }
    int num=0;
    while(*s)
    {
        num=((*s)-'0')+num*10;
        s++;
    }
    return num*sign;
}

int main(int args,char ** argv) {
    printf("%c",argv[2][0]);
    time_t usrTime = parseUsrTime(a2i(argv[8]), a2i(argv[7]), a2i(argv[6]),a2i(argv[3]),a2i(argv[4]),a2i(argv[5]));
    getFiles(argv[1],argv[2][0],usrTime);
}
