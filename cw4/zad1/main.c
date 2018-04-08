#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

pid_t childpid;

void printnclose(char* c){
    printf("%s",c);
    exit(1);
}

int finishChild(){
    pid_t tst = waitpid(childpid, NULL, WNOHANG);
    if(!tst) {
        kill(childpid, SIGKILL);
        return 1;
    }
    return 0;
}

void runProcess(){
    int pid = fork();
    if(pid<0)
        printnclose("An error occurred in fork\n");
    if(pid==0){
        signal(SIGINT, SIG_IGN);
        signal(SIGTSTP, SIG_IGN);

        if(execlp("./prntdt.sh",NULL) == -1)
            printnclose("An error occurred on script run.\n");
    }
    if(pid>0)
        childpid = pid;
}

void finish(int i){
    printf(": SIGINT - ending programme run\n");
    finishChild();
    exit(0);
}

void toggleChild(int i) {
    if (finishChild()) {
        printf(": Press CTRL+Z to continue, CTRL+C to finish\n");
    } else {
        printf(": SIGSTP - resuming programme run\n");
        runProcess();
    }
}

int main(int args, char * argv[]) {

    if(signal(SIGINT,finish)==SIG_ERR)
        printnclose("Couldn't intercept SIGINT signal\n");

    struct sigaction act;
    act.sa_handler = toggleChild;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    if(sigaction(SIGTSTP, &act, NULL)==-1){
        printnclose("Couldn't modify SIGTSTP disposition\n");
    }
    runProcess();
    while(1) pause();
}