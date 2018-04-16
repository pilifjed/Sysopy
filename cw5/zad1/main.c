#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/errno.h>
#include <ctype.h>
#include <memory.h>

#define OPTION_SIZE 25
#define MAX_COMMANDS 10

void runN(int* pid,int N){
    pid[0]=getpid();
    for(int i=1;i<N;i++){
        if(pid[0]==getpid()) {
            int p = fork();
            if (p==0)
                pid[i] = getpid();
        }
        else
            break;
    }
}

void closeExcept(int fd[2][2], int N, int fd1[2], int fd2[2]){
    for(int i=0;i<N-1;i++) {
        if(fd[i]!=fd1 && fd[i]!=fd2) {
            close(fd[i][0]);
            close(fd[i][1]);
        }
    }
}

void createPipes(int allfd[MAX_COMMANDS][2], int N){
    for(int i=0;i<N-1;i++){
        pipe(allfd[i]);
    }
}

void pipeStart(int pid, int fd1[2], int allfd[MAX_COMMANDS][2], int N, char* command[OPTION_SIZE]){
    if(pid == getpid()){

        closeExcept(allfd, N, fd1, NULL);

        close(fd1[0]);
        dup2(fd1[1], STDOUT_FILENO);
        close(fd1[1]);
        execvp(command[0],command);
    }
}

void pipeMid(int pid, int fd1[2], int fd2[2], int allfd[MAX_COMMANDS][2], int N, char* command[OPTION_SIZE]){
    if(pid == getpid()){

        closeExcept(allfd, N, fd1, fd2);

        close(fd1[1]);
        close(fd2[0]);

        dup2(fd1[0], STDIN_FILENO);
        close(fd1[0]);

        dup2(fd2[1], STDOUT_FILENO);
        close(fd2[1]);

        execvp(command[0], command);
    }
}

void pipeEnd(int pid,int fd2[2], int allfd[MAX_COMMANDS][2], int N, char* command[OPTION_SIZE]){
    if(pid == getpid()){

        closeExcept(allfd, N, fd2,NULL);

        close(fd2[1]);
        dup2(fd2[0], STDIN_FILENO);
        close(fd2[0]);
        execvp(command[0],command);
    }
}

void runPipes(int N, char* commands[MAX_COMMANDS][OPTION_SIZE]){
    int fd[MAX_COMMANDS][2];

    createPipes(fd,N);

    int pid[N];
    runN(pid,N);

    pipeStart(pid[0],fd[0],fd,N,commands[0]);

    for(int i=1;i<N-1;i++){
        pipeMid(pid[i],fd[i-1],fd[i],fd,N,commands[i]);
    }
    pipeEnd(pid[N-1],fd[N-2],fd,N,commands[N-1]);
}

int parsePipeLine(char * ret [MAX_COMMANDS][OPTION_SIZE],char * buffer, size_t bsize){
    int poz = 0;
    char * tmp;
    for(int i=0;i<bsize;i++){
        if(isspace(buffer[i])){
            buffer[i]=' ';
        }
    }
    int cmd=0,opt=0;
    tmp = strtok(buffer," ");
    ret[cmd][opt]=tmp;
    opt++;
    while(1) {
        tmp = strtok(NULL, " ");
        if (tmp == NULL)
            break;
        if (tmp[0] == '|') {
            opt++;
            ret[cmd][opt] = NULL;
            cmd++;
            opt = 0;
        } else {
            ret[cmd][opt] = tmp;
            opt++;
        }
    }
    opt++;
    return opt;
}

int parseOtherLine(char * ret [OPTION_SIZE],char * buffer, size_t bsize){
    int poz = 0;
    char * tmp;
    for(int i=0;i<bsize && buffer[i] !='\0';i++){
        if(isspace(buffer[i])){
            buffer[i]=' ';
        }
    }
    int cmd=0,opt=0;
    tmp = strtok(buffer," ");
    ret[opt]=tmp;
    opt++;
    while(1){
        tmp = strtok(NULL," ");
        if(tmp==NULL) {
            ret[opt]=NULL;
            return opt;
        }
        ret[opt]=tmp;
        opt++;
    }
}

int isPipeLine(char * buffer, size_t bsize){
    for(int i=0;i<bsize;i++) {
        if(buffer[i]=='|')
            return 1;
    }
    return 0;
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
        char * ret [MAX_COMMANDS][OPTION_SIZE];
        int N;
        if(isPipeLine(bval,bsize)){
            int stat;
            N  = parsePipeLine(ret,bval,bsize);
            runPipes(N,ret);
            wait(&stat);

        } else {
            N = parseOtherLine(ret[0], bval, bsize);
            int pid = vfork();
            int stat;
            if (pid == 0) {
                if (execvp(ret[0][0], ret[0])){
                    printf("An error occurred while running command");
                    exit(1);
                }
            }
            wait(&stat);
        }
        lct++;
    }
    fclose(ofile);
    free(bval);
}