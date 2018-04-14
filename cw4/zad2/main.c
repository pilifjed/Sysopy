#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_CHILDREN_COUNT 50
#define SIGMSG SIGRTMAX //SIGUSR1

pid_t signalsGot[MAX_CHILDREN_COUNT];
pid_t processes[MAX_CHILDREN_COUNT + 1];
int sigct = 0;
int K, N;

void actionTwo(int i)
{
}

void runChildren(int count)
{
    processes[0] = getpid();
    for (int i = 1; i < count + 1; i++)
    {
        int pid = fork();
        if (pid < 0)
        {
            printf("An error occurred while using fork function");
            exit(1);
        }
        if (pid == 0)
        {
            srand(time(NULL));
            printf("RUN: %d\n", getpid());
            signal(SIGUSR2, actionTwo);
            processes[i] = getpid();
            int slp = rand() % 9 + 1;
            sleep(slp);

            union sigval val;
            val.sival_int = processes[i];
            sigqueue(processes[0], SIGMSG, val);

            sigset_t mask;
            sigemptyset(&mask);
            sigaddset(&mask, SIGUSR2);
            int sig;
            sigwait(&mask, &sig);

            int end = rand() % (SIGRTMAX - SIGRTMIN - 1) + SIGRTMIN - 1;
            kill(processes[0], end);
            _exit(slp);
        }
    }
}

void actionOne(int sig, siginfo_t *info, void *ucontext)
{
    printf("RECEIVED SIGMSG FROM %d\n", info->si_pid);
    if (sigct < K)
    {
        sigct += 1;
        signalsGot[sigct - 1] = info->si_value.sival_int;
        printf("STORED: %d\n", info->si_value.sival_int);
    }
    if (sigct > K)
    {
        printf("SENT: %d\n", info->si_value.sival_int);
        kill(info->si_value.sival_int, SIGUSR2);
    }
    if (sigct == K)
    {
        printf("BEGIN SENDING SIGNALS TO STORED PID\n");
        for (int i = 0; i < K; i++)
        {
            printf("SENT: %d\n", signalsGot[i]);
            kill(signalsGot[i], SIGUSR2);
        }
        printf("END SENDING SIGNALS TO STORED PID\n");
        sigct += 1;
    }
}
void actionThree(int sig, siginfo_t *info, void *ucontext)
{
    printf("RECEIVED SIGMIN+%d FROM %d\n", sig, info->si_pid);
}

void actionFour(int sig, siginfo_t *info, void *ucontext)
{
    printf("CHILD %d FINISHED RUNNING WITH EXIT STATUS %d\n", info->si_pid, info->si_status);
    N = N - 1;
    if (N == 0)
    {
        _exit(0);
    }
    for (int i = 1; i < N + 1; i++)
    {
        if (processes[i] == info->si_pid)
            processes[i] = -1;
    }
}
void actionSix(int sig, siginfo_t *info, void *ucontext)
{
    printf("PARENT RECEIVED SIGINT\n");
    for (int i = 1; i < N + 1; i++)
    {
        if (processes[i] != -1)
        {
            printf("KILLED CHILD %d", processes[i]);
            kill(processes[i], SIGKILL);
            waitpid(processes[i], NULL, 0);
        }
    }
    exit(0);
}

int main(int args, char *argv[])
{
    if (args != 3)
    {
        printf("Error while parsing arguments: args=%d", args);
    }

    N = atoi(argv[1]);
    K = atoi(argv[2]);

    for (int i = 0; i < N; i++)
    {
        processes[i] = -1;
    }

    struct sigaction act1;
    act1.sa_flags = SA_SIGINFO;
    act1.sa_sigaction = actionOne;
    sigemptyset(&act1.sa_mask);
    sigaction(SIGMSG, &act1, NULL);

    for (int i = SIGRTMIN; i < SIGRTMAX; i++)
    {
        act1.sa_sigaction = actionThree;
        sigaction(i, &act1, NULL);
    }
    act1.sa_sigaction = actionFour;
    sigaction(SIGCHLD, &act1, NULL);
    act1.sa_sigaction = actionSix;
    sigaction(SIGINT, &act1, NULL);

    runChildren(N);
    int status;
    while (1)
    {
        wait(&status);
    }
}