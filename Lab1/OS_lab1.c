#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>


void read_files(int *pid_array, int size){

    printf("==========================================\n");
    //printf("%d", *pid_array);

    for(int i=0; i< size;++i){
        char p[10];
        sprintf(p, "%d", pid_array[i]);

        char filename[20];
        strcpy(filename, "output_");
        strcat(filename, p);
        strcat(filename,".txt");

        //printf("%s", filename);

        int fd = open (filename, O_RDONLY);
        if(fd < 0){
            perror("read");
            exit(1);
        }

        char* read_buf = (char *)malloc(100*sizeof(char));
        int sz = read(fd, read_buf,100);

        read_buf[sz]='\0';

        printf("The file %s has the below content:\n%s\n", filename, read_buf);

        close(fd);
    }
}

int write_output(int cpid, int ppid){
    char c[10];
    sprintf(c, "%d", cpid);
    char p[10];
    sprintf(p, "%d", ppid);

    char char_buffer[20];
    strcpy(char_buffer, "output_");
    strcat(char_buffer, c);
    strcat(char_buffer,".txt");
    
    const char *filename = char_buffer;
    //Buffer to write to the file Child PID: <PID>, Parent PID: <PPID>
    char buf[100];
    strcpy(buf,"Child PID: ");
    strcat(buf, c);
    strcat(buf, ", Parent PID: ");
    strcat(buf, p);


    int fd = open(filename, O_CREAT | O_APPEND | O_WRONLY | O_RDONLY , 0644);
    
    if(fd==-1){
        perror("open");
        return 1;
    }

    if(write(fd, buf,strlen(buf)) < strlen(buf)){
        perror("write");
        return 1;
    }

    close(fd);
    return 0;

}

int main(int argc, char *argv[]){

    if(argc != 2 || argv[1]=="--help") {
        printf("Usage: ./a.out num_of_processes\n");
        return 1;
    }

    int status;

    int num_of_processes = *argv[1] - '0';

    int *pid_array = (int*) malloc(num_of_processes*sizeof(int));
    
    pid_t parentID=getpid();

    for(int i=0; i<num_of_processes; ++i){
        
        pid_t child = fork();

        if(child <0) {
            printf("Error with childbirth!\nExiting program...");
            return -1;
        }

        //What every child must do
        if(child ==0) {
            //Display a message with its PID
            
            printf("Created Child Process with PID: %d\n", getpid());

            //Write to its file
            if (write_output(getpid(), getppid())!=0) {
                printf("Error in writing to the file - Exiting\n");
                return 0;
            }
            exit(0);
        }
        //Parent code
        else {
            //Load to the allocatd memory the pIDs of their children
            pid_array[i]=child;
        }
    }

    while(wait(&status) >0);
    read_files(pid_array, num_of_processes);

    free(pid_array);
    return 0;
}