#include <stdio.h>
#include <zconf.h>
#include <stdlib.h>
#include <ctype.h>

#define OPTIONS_NO 11
#define OPTION_SIZE 25

struct Args{
    int opts;
    char ** optv;
};

struct Args initArgs(){
    struct Args args;
    args.opts=0;
    args.optv=malloc((OPTIONS_NO+1)* sizeof(char*));
    for(int i=0;i<OPTIONS_NO+1;i++){
        args.optv[i]=NULL;
    }
    return args;
}

struct Args parse(char * buffer, size_t bsize){
    struct Args args=initArgs();
    int poz=0;
    while(poz<bsize){
        if(!isspace(buffer[poz])){
            int ct=0;
            args.optv[args.opts]= malloc(OPTION_SIZE*sizeof(char));
            while(!isspace(buffer[poz])){
                args.optv[args.opts][ct]=buffer[poz];
                ct++;
                poz++;
            }
            args.opts++;
        }
        poz++;
    }
    return args;
}

int main(int args, char * argv[]) {
    if(args != 2){
        printf("Bad syntax! Please specify file path only.");
        return 1;
    }
    FILE* ofile = fopen(argv[1],"r");
    char* bval = NULL;
    size_t bsize = 0;
    ssize_t bts;
    int lct = 0;
    while((bts = getline(&bval,&bsize,ofile)) != -1){ //automatic allocation
        bsize = (size_t) bts;
        struct Args parsedLine = parse(bval,bsize); 
        int pid = vfork();
        int stat;
        if(pid == 0){
            if (execvp(parsedLine.optv[0],parsedLine.optv)){
                printf("An error occurred while running command");
                exit(1);
            }
        }
        wait(&stat);
        if(WIFEXITED(stat) && WEXITSTATUS(stat)){
            printf("LINE: %d COMMAND: %s EXITED WITH STATUS: %d\n",lct,parsedLine.optv[0],WEXITSTATUS(stat));
            exit(1);
        }
        if(WIFSIGNALED(stat)){
            printf("LINE: %d COMMAND: %s TERMINATED WITH SIGNAL: %d\n",lct,parsedLine.optv[0],WTERMSIG(stat));
            exit(1);
        }
        lct++;
    }
    fclose(ofile);
    free(bval);
}