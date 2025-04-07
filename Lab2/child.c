#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <string.h>

int variable=0;

void alarm_handler() {
    time_t current_time;
    struct tm *time_info;
    char time_string[9]; // HH:MM:SS format

    time(&current_time);
    time_info = localtime(&current_time);

    strftime(time_string, sizeof(time_string), "%H:%M:%S", time_info);
    printf("[%s] I'm PID: %d, my variable is: %d\n", time_string, getpid(), variable);
    alarm(10);
}
void usr1_handle(){
    variable++;
}

void usr2_handle(){
    variable--;
}

void term_handle(){
    exit(1);
}

int main() {
    signal(SIGALRM, alarm_handler);
    alarm(10);

    struct sigaction usr1;
    struct sigaction usr2;
    struct sigaction term;

    usr1.sa_handler = usr1_handle;
    usr2.sa_handler = usr2_handle;
    term.sa_handler = term_handle;

    sigaction(SIGUSR1,&usr1,NULL);
    sigaction(SIGUSR2,&usr2,NULL);
    sigaction(SIGTERM,&term,NULL);


    while (1) {
        pause();
    }

    return 0;
}