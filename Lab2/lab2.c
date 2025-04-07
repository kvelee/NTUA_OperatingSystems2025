#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>

int *pid_array=NULL;
int number_of_children;

void usr1_handle() {
    for (int i = 0; i < number_of_children; ++i) {
        if (kill(pid_array[i], SIGUSR1) < 0) {
            perror("kill SIGUSR1 failed");
        }
    }
}

void usr2_handle() {
    for (int i = 0; i < number_of_children; ++i) {
        if (kill(pid_array[i], SIGUSR2) < 0) {
            perror("kill SIGUSR2 failed");
        }
    }
}

void term_handle() {
    for (int i = 0; i < number_of_children; ++i) {
        if (kill(pid_array[i], SIGTERM) < 0) {
            perror("kill SIGTERM failed");
        }
    }
}

void chld_handle() {
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WUNTRACED)) > 0) {
        
        if (WIFEXITED(status)) {
           

            for (int i = 0; i < number_of_children; ++i) {
                if (pid_array[i] == pid) {
                    pid_t new_child = fork();
                    if (new_child == 0) {
                        char *args[] = {"./child", NULL};
                        execv(args[0], args);
                        perror("execv failed"); 
                        exit(1);
                    } else {
                        pid_array[i] = new_child;
                    }
                    break;
                }
            }
        } else if (WIFSTOPPED(status)) {
            printf("Child process %d was paused. Sending SIGCONT to resume it.\n", pid);
            if(kill(pid, SIGCONT)<0) {
                perror("kill SIGCONT failed");
            }
        }
    }

    if(sizeof(pid_array)/sizeof(int) != number_of_children){
        printf("Missing Child(ren)\n");
    }
}


void print_children(int *array){
    
    int size = sizeof(array)/sizeof(int);

    printf("The process-children created were:\n");

    for(int i=0;i<size;++i){
        printf("Child_%d with PID: %d\n", i+1, array[i]);
    }

}

int main(int argc, char *argv[]) {
    
    if(argc !=2) {
        printf("Usage: %s <number>\n", argv[0]);
        return 1;
    }

    number_of_children = atoi(argv[1]);

    if(number_of_children <=0) {
        printf("Please use a valid number >=1\n");
        return 1;
    }

    pid_array = (int *)malloc(number_of_children * sizeof(int));
    if (pid_array == NULL) {
        perror("Failed to allocate memory");
        return 1;
    }
    


    printf("Parent process PID: %d\n", getpid());

    pid_t child;

    for(int i=0;i<number_of_children;++i){
        child = fork();

        if (child < 0) {
            perror("fork failed");
            free(pid_array);
            exit(1);
        }
        else if (child != 0) {
            pid_array[i]=child;
        }
        else {
            char *args[] = {"./child", NULL};
            execv(args[0], args);
            perror("execv failed");
            exit(1);
        }
    }

    if(child!=0) print_children(pid_array);

    struct sigaction usr1;
    struct sigaction usr2;
    struct sigaction term;
    struct sigaction chld;

    usr1.sa_handler = usr1_handle;
    usr2.sa_handler = usr2_handle;
    term.sa_handler = term_handle;
    chld.sa_handler = chld_handle;

    if (sigaction(SIGUSR1, &usr1, NULL) < 0) {
        perror("sigaction SIGUSR1 failed");
        free(pid_array);
        exit(1);
    }
    if (sigaction(SIGUSR2, &usr2, NULL) < 0) {
        perror("sigaction SIGUSR2 failed");
        free(pid_array);
        exit(1);
    }
    if (sigaction(SIGTERM, &term, NULL) < 0) {
        perror("sigaction SIGTERM failed");
        free(pid_array);
        exit(1);
    }
    if (sigaction(SIGCHLD, &chld, NULL) < 0) {
        perror("sigaction SIGCHLD failed");
        free(pid_array);
        exit(1);
    }

    while(1) sleep(5);   //Μη τερματισμός του προγράμματος

    return 0;
}