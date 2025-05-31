/* ICCS227: Project 1: icsh
 * Name: Teeratass Peysantiwong
 * StudentID: 6480651
 */

#include "stdio.h"
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define MAX_CMD_BUFFER 255
int prev_status = 0;
volatile pid_t pid_fg=0;
void sigtstp_handler(int signal){
    if (pid_fg>0) kill(pid_fg,SIGTSTP);
}
void sigint_handler(int signal){
    if (pid_fg>0) kill(pid_fg,SIGINT);
}
int main(int argc, char * argv[]) {
    char prev[MAX_CMD_BUFFER]="";
    char buffer[MAX_CMD_BUFFER];
    FILE *input=stdin;
    int check_script=0;
    if (argc==2){
        input=fopen(argv[1],"r");
        if(!input){
            perror("Error while opening the file");
            return 1;
        }
        check_script=1;
    }
    signal(SIGINT, sigint_handler);
    signal(SIGTSTP, sigtstp_handler);
    while (1) {
        if(!check_script){
            printf("icsh $ ");
            fflush(stdout);
        }
        if(fgets(buffer, MAX_CMD_BUFFER,input)== NULL){
            if (check_script) break;
            else{
            printf("\n");
            break;
            }
        }
        buffer[strcspn(buffer, "\n")]=0;
        if(strlen(buffer)==0){
            continue;
        }
        if (strcmp(buffer, "!!") == 0) {
            if (strlen(prev) == 0) {
                continue;  
            }
            if (!check_script){
            printf("%s\n", prev);
        }

            strcpy(buffer, prev);  
        } else {
            strcpy(prev, buffer);  
        }
        //fgets(buffer, 255, stdin);
        // printf("you said: %s\n", buffer);
        char command[20][255];
        int count=0;
        char *ptr=buffer;
        while(sscanf(ptr,"%s",command[count])==1){
            ptr+=strlen(command[count]);
            while (*ptr==' ') ptr++;
            count++;
        }
        if(((strcmp(command[0],"echo")==0 && count==2)) && (strcmp(command[1],"$?")==0)){
             printf("%d\n", prev_status);
            prev_status = 0;
            continue;
        }
        else if(strcmp(command[0],"echo")==0){
            for(int i=1;i<count;i++){
                // printf("%s ", command[i]);
                 printf("\033[1;32m%s\033[0m",command[i]);
            }
            printf("\n");
        }
        
        else if (strcmp(command[0], "exit") == 0) {
            int code = 0;
            if (count >= 2) {
                code = atoi(command[1]) % 256;
            }
            // printf("bye\n");
            if (check_script){
                printf("$ echo $?\n%d\n", code);  
                fclose(input);
            }
            if (!check_script && count<2){
            printf("\033[1;31mbye\033[0m\n");
        }
            if (!check_script && count >= 2){
                printf("\033[1;31m%d\033[0m\n", code);
            }
            return code;
        }
        // else if (!check_script) {
        //     printf("\033[1;33mbad command\033[0m\n");
        //     // printf("bad command\n");
        // }
        else {
            //condition pid<0 !pid()->0 pid()->1
            //idea from the resource at the buttom of assignment
            char *prog_argv[count];
            for (int i = 0; i < count; i++) {
                prog_argv[i] = command[i];
            }
            prog_argv[count]=NULL;
            pid_t pid = fork();
            if (pid < 0) {
                perror("fork failed");
                continue;
            } else if (!pid) {
                 /* This is the child */ 
                execvp(prog_argv[0], prog_argv);
                perror("exec failed");
                exit(1);
            } else if (pid){
                /* 
                * We're in the parent; let's wait for the child to finish*/
                int status;
                waitpid(pid, &status, 0);
            }
        }
    }

    if (check_script) {
        fclose(input);
    }
    return 0;
}
