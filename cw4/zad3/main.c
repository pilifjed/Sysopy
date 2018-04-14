#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

int ps = 0, cr = 0, pr = 0;

void sigcld1(int i) {
    cr++;
    kill(getppid(), i);
}

void sigcld2(int i) {
    printf("->child: %d\n",cr);
    exit(0);
}

void ct(int i) {
    pr++;
}

int main(int args, char *argv[]) {
    if (args != 3) {
        printf("Wrong argument format");
        exit(1);
    }

    int cid = fork();

    if (cid < 0) {
        exit(1);
    } else if (cid == 0) {
        signal(SIGUSR1, sigcld1);
        signal(SIGUSR2, sigcld2);
        signal(SIGRTMAX,sigcld1);
        signal(SIGRTMIN,sigcld2);
        while(1) pause();
    } else {
        sleep(1);
        signal(SIGUSR1, ct);
        signal(SIGRTMAX,ct);
        int s;
        switch (atoi(argv[2])) {
            case 1:
                for (int i = 0; i < atoi(argv[1]); i++) {
                    kill(cid, SIGUSR1);
                    ps++;
                }
                printf("parent->: %d\n",ps);
                kill(cid, SIGUSR2);
                wait(&s);
                printf("parent<-: %d\n", pr);
                exit(0);
            case 2:
                for (int i = 0; i < atoi(argv[1]); i++) {
                    kill(cid, SIGUSR1);
                    ps++;
                    sleep(1);
                }
                printf("parent->: %d\n",ps);
                kill(cid, SIGUSR2);
                wait(&s);
                printf("parent<-: %d", pr);
                exit(0);
            case 3:
                for (int i = 0; i < atoi(argv[1]); i++) {
                    kill(cid, SIGRTMAX);
                    ps++;
                }
                printf("parent->: %d\n",ps);
                kill(cid, SIGRTMIN);
                wait(&s);
                printf("parent<-: %d\n", pr);
                exit(0);
            default:
                printf("Wrong second argument");
                exit(1);
        }
    }
}