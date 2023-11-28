#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void alarm_handler(int signo){
    printf("sleep end\n");
    
    printf("send signal quit\n");

    kill(getpid(), SIGQUIT);
    printf("blocked\n");

    raise(SIGUSR1);

    kill(getpid(), SIGQUIT);
    printf("blocked\n");
}

void handler(int sig) {
    sigset_t set;

    sigemptyset(&set);
    sigaddset(&set, SIGQUIT);

    sigprocmask(SIG_UNBLOCK, &set, NULL);
    printf("UNBLOCK SIGQUIT\n");
}

int main() {
    struct sigaction act_alarm, act_usr1;
    int secs, i = 0;
    sigset_t set;

    sigemptyset(&set);
    sigaddset(&set, SIGQUIT);
    sigprocmask(SIG_BLOCK, &set, NULL);

    act_usr1.sa_handler = handler;
    sigaction(SIGUSR1, &act_usr1, NULL);
    
    act_alarm.sa_handler = alarm_handler;
    sigaction(SIGALRM, &act_alarm, NULL);
    
    printf("enter secs : ");
    scanf("%d", &secs);
    
    printf("sleep %d seconds\n", secs);

    alarm(secs);
    pause();
}