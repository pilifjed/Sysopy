#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

int main(int argc, char * argv[]){
    if(argc!=2){
        printf("Wrong arguments syntax!");
        exit(1);
    }
    if(mkfifo(argv[1],0666) == -1){
        printf("Creating FIFO went wrong! %s.\n", strerror(errno));
        exit(1);
    }
    FILE * handle;
    handle = fopen(argv[1],"r");
    if(handle == NULL){
        printf("An error occurred while opening FIFO! %s.\n", strerror(errno));
    }
    char* lineptr=NULL;
    size_t n=0;
    while(getline(&lineptr,&n,handle) != -1){
        printf("%s",lineptr);
    }
    fclose(handle);
    remove(argv[1]);
    return 0;
}