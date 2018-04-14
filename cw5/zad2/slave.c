#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <zconf.h>
#include <sys/time.h>


void fsave(){

}

int main(int argc, char * argv[]){
    if(argc!=3){
        printf("Wrong arguments syntax!");
        exit(1);
    }

    FILE * fhandle;
    fhandle = fopen(argv[1],"w");
    if(fhandle == NULL){
        printf("An error occurred while opening FIFO! Check if correct path is specified.");
    }
    printf("%d\n",getpid());

    FILE * dthandle;
    char* lineptr=NULL;
    size_t n=0;
    char newline[50];
    for(int i=0;i<atoi(argv[2]);i++){
        srand(time(NULL));
        dthandle = popen("date","r");
        getline(&lineptr,&n,dthandle);
        pclose(dthandle);
        sprintf(newline,"[PID: %d]: %s",getpid(),lineptr);
        fputs(newline,fhandle);
        sleep(rand()%3+2);
    }
    fclose(fhandle);
    return 0;
}