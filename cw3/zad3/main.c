#include <stdio.h>
#include <zconf.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/time.h>

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

    for(int i=0;i<bsize;i++){
        if(isspace(buffer[i])){
            buffer[i] = ' ';
        }
    }

    while(poz<bsize){
        if(buffer[poz]!=' '){
            int ct=0;
            args.optv[args.opts]= malloc(OPTION_SIZE*sizeof(char));
            while(buffer[poz]!=' '){
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

void printArgs(struct Args a){
    printf(" COMMAND: ");
    for(int i=0;i<a.opts;i++){
        printf("%s ",a.optv[i]);
    }
}

int main(int args, char * argv[]) {
    if(args != 4){
        printf("Bad syntax! Please specify file path only.\n");
        return 1;
    }
    FILE* ofile = fopen(argv[1],"r");
    char* bval = NULL;
    size_t bsize = 0;
    ssize_t bts;

    struct rlimit tlim, mlim ;
    tlim.rlim_cur = ((rlim_t) (atoi(argv[2])));
    tlim.rlim_max = tlim.rlim_cur;
    mlim.rlim_cur = ((rlim_t) atoi(argv[3])) << 20; //default linux vmem limit is 8 KiB; argv[3] stores mem in KiB so conversion to bytes is necessary
    mlim.rlim_max = mlim.rlim_cur;

    int lct = 0;
    while((bts = getline(&bval,&bsize,ofile)) != -1){ //automatic allocation
        bsize = (size_t) bts;
        struct Args parsedLine = parse(bval,bsize);
        struct rusage rs, re;

        if(getrusage(RUSAGE_CHILDREN,&rs)){
            printf("An error ocurred while obtaining process information\n");
            exit(1);
        }
        int pid = vfork();
        int stat;
        if(pid == 0){
            setrlimit(RLIMIT_CPU, &tlim);
            setrlimit(RLIMIT_AS, &mlim);
            struct rlimit tmp;
//            getrlimit(RLIMIT_CPU,&tmp);
//            printf("CPU: %d ",(int) tmp.rlim_max);
//            getrlimit(RLIMIT_AS,&tmp);
//            printf("AS: %d\n ",(int) tmp.rlim_max);

            if (execvp(parsedLine.optv[0],parsedLine.optv)){
                printf("An error occurred while running command\n");
                exit(1);
            }
        }
        wait(&stat);
        if(WIFEXITED(stat) && WEXITSTATUS(stat)){
            printf("LINE: %d", lct);
            printArgs(parsedLine);
            printf("EXITED WITH STATUS: %d\n",WEXITSTATUS(stat));
            exit(1);
        }
        if(WIFSIGNALED(stat)){
            printf("LINE: %d", lct);
            printArgs(parsedLine);
            printf("TERMINATED WITH SIGNAL: %d\n",WTERMSIG(stat));
            exit(1);
        }
        if(getrusage(RUSAGE_CHILDREN,&re)){
            printf("An error ocurred while obtaining process information\n");
            exit(1);
        }
        printf("LINE: %d",lct);
        printArgs(parsedLine);
        struct timeval ut, st;
        timersub(&re.ru_utime,&rs.ru_utime,&ut);
        timersub(&re.ru_stime,&rs.ru_stime,&st);
        printf("USER TIME: %d.%ds ", (int) ut.tv_sec, (int) ut.tv_usec);
        printf("SYSTEM TIME: %d.%ds\n\n", (int) st.tv_sec, (int) st.tv_usec);
        lct++;
    }
    fclose(ofile);
    free(bval);
}
