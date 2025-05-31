/* ICCS227: Project 1: icsh
 * Name: Teeratass Peysantiwong
 * StudentID: 6480651
 */

#include "stdio.h"
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h>
#include "game.h"
#define MAX_CMD_BUFFER 255
int prev_status = 0;
volatile pid_t pid_fg=0;
#define JMAX 64
int jobcounter=0;
int jobid=1;
typedef struct{
    int id;
    pid_t pid;
    char command[MAX_CMD_BUFFER];
    char status[16];
}Job;
Job jobs[JMAX];

int find_job_with_pid(int pid){
    for (int i=0;i<jobcounter;i++){
        if(jobs[i].pid == pid) {
            return i;
        }
    }
    return -1;
}
int find_job_with_id(int id){
    for (int i = 0; i < jobcounter; i++) {
        if (jobs[i].id == id) return i;
    }
    return -1;
}



void sigtstp_handler(int signal){
    if (pid_fg>0) kill(pid_fg,SIGTSTP);
}
void sigint_handler(int signal){
    if (pid_fg>0) kill(pid_fg,SIGINT);
}
//https://stackoverflow.com/questions/33508997/waitpid-wnohang-wuntraced-how-do-i-use-these
//https://www.ibm.com/docs/en/zvm/7.3?topic=descriptions-waitpid-wait-specific-child-process-end
//https://github.com/marioskogias/os/blob/master/exe4/sigchld-example.c for reference only
//https://support.sas.com/documentation/onlinedoc/ccompiler/doc/lr2/waitpid.htm
//https://www.youtube.com/watch?v=kCGaRdArSnA goat


void sigchld_handler(int signal){
    int status; 
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0) {
        int idx = find_job_with_pid(pid);
        if (idx >= 0) {
            if (WIFEXITED(status) || WIFSIGNALED(status)) {
                if (strcmp(jobs[idx].status, "Running") == 0) {
                    printf("\n[%d]+  Done\t\t%s\n", jobs[idx].id, jobs[idx].command);
                    fflush(stdout);
                    strcpy(jobs[idx].status, "Done");
                }
            } else if (WIFSTOPPED(status)) {
                printf("\n[%d]+  Stopped\t\t%s\n", jobs[idx].id, jobs[idx].command);
                fflush(stdout);
                strcpy(jobs[idx].status, "Stopped");
            }
        }
    }
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
    signal(SIGCHLD, sigchld_handler);
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
        // char command[20][255];
        // int count=0;
        // char *ptr=buffer;
        // while(sscanf(ptr,"%s",command[count])==1){
        //     ptr+=strlen(command[count]);
        //     while (*ptr==' ') ptr++;
        //     count++;
        // }
        char command[20][255];
        int count=0;
        char *ptr=buffer;
        int bg=0;
        //issuess
        // while(sscanf(ptr,"%s",command[count])==1){
        //     ptr+=strlen(command[count]);
        //     while (*ptr==' ') ptr++;
        //     count++;
        // }
        char *inp = strtok(buffer," ");
        while (inp!=NULL){
            if(inp[0]=='<'||inp[0]=='>'){
                if(strlen(inp)>1){
                    command[count][0]=inp[0];
                    command[count][1]='\0';
                    count++;
                    strcpy(command[count], inp+ 1);
                    count++;
                }
                else {
                    strcpy(command[count], inp);
                    count++;
                }
            }
            else {
            strcpy(command[count], inp);
            count++;
             }
            inp = strtok(NULL, " ");
        }

        if (strcmp(command[count-1], "&")==0){
            bg=1;
            count--;
        }
        if(((strcmp(command[0],"echo")==0 && count==2)) && (strcmp(command[1],"$?")==0)){
             printf("%d\n", prev_status);
            prev_status = 0;
            continue;
        }

        // else if(strcmp(command[0],"echo")==0){
        //     for(int i=1;i<count;i++){
        //         // printf("%s ", command[i]);
        //          printf("\033[1;32m%s\033[0m",command[i]);
        //     }
        //     printf("\n");
        // }
        
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
        // else if (strcmp(command[0], "wordle") == 0) {
        //     wordle_game();
        //     continue;
        //}
        if(strcmp(command[0], "jobs")==0){
            for(int i=0;i<jobcounter;i++){
                if (strcmp(jobs[i].status, "Done") != 0) {
                    printf("[%d]%c  %s\t\t%s\n", jobs[i].id,
                           (i == jobcounter - 1) ? '+' : '-',
                           jobs[i].status, jobs[i].command);
                }
            }
            continue;
        }
        // else if (!check_script) {
        //     printf("\033[1;33mbad command\033[0m\n");
        //     // printf("bad command\n");
        // }
        //https://www.geeksforgeeks.org/c-atoi-function/ https://www.tutorialspoint.com/c_standard_library/c_function_atoi.htm
        if (strcmp(command[0], "fg") == 0 && count >= 2){
            int job_id=atoi(command[1]+1);
            int findingjob=find_job_with_id(job_id);
            if (findingjob < 0) {
                printf("fg: job not found: %s\n", command[1]);
            } else {
                printf("%s\n", jobs[findingjob].command);
                kill(jobs[findingjob].pid, SIGCONT);
                pid_fg = jobs[findingjob].pid;
                int status;
                waitpid(jobs[findingjob].pid, &status, WUNTRACED);
                pid_fg = 0;
                if (WIFSTOPPED(status)) {
                    strcpy(jobs[findingjob].status, "Stopped");
                } else {
                    for (int j = findingjob; j < jobcounter - 1; j++) {
                        jobs[j] = jobs[j + 1];
                    }
                    jobcounter--;
                }
            }
            continue;

        }
        if (strcmp(command[0], "bg") == 0 && count >= 2) {
            int job_id=atoi(command[1]+1);
            int findingjob = find_job_with_id(job_id);
            if (findingjob < 0) {
                printf("bg: job not found: %s\n", command[1]);
            } else if (strcmp(jobs[findingjob].status, "Stopped") == 0) {
                kill(jobs[findingjob].pid, SIGCONT);
                strcpy(jobs[findingjob].status, "Running");
                printf("[%d]+ %s &\n", jobs[findingjob].id, jobs[findingjob].command);
            }
            continue;
        }
        else {
            //condition pid<0 !pid()->0 pid()->1
            //idea from the resource at the buttom of assignment
            int proarg_index=0;
            char *prog_argv[count+1];
            char *ifile=NULL; char *ofile=NULL;
            for (int i = 0; i < count; i++) {
                if (strcmp(command[i],">")==0 && i+1 < count){
                    ofile=command[i+1]; i++;
                }
                else if (strcmp(command[i],"<")==0 && i+1 < count){
                    ifile=command[i+1]; i++;
                }
                else{
                    prog_argv[proarg_index++] = command[i];
                }
                
            }
            prog_argv[proarg_index]=NULL;
            pid_t pid = fork();
            if (pid < 0) {
                perror("fork failed");
                continue;
            } 
            else if (!pid) {

                signal(SIGINT,SIG_DFL);
                signal(SIGTSTP,SIG_DFL);
                 /* This is the child */ 
                 //https://www.quora.com/What-is-the-file-descriptor-What-are-STDIN_FILENO-STDOUT_FILENO-and-STDERR_FILENO confusing but surei guess
                 
                if (ifile != NULL) {
                    int in = open(ifile, O_RDONLY);
                    if (in < 0) {
                        perror("Failed to open the input file");
                        exit(1);
                    }
                    dup2(in, STDIN_FILENO);
                    close(in);
                }
                //https://pubs.opengroup.org/onlinepubs/007904875/basedefs/fcntl.h.html
                //https://stackoverflow.com/questions/14003466/how-can-i-read-and-write-from-files-using-the-headers-fcntl-h-and-unistd-h
                //https://pubs.opengroup.org/onlinepubs/7908799/xsh/open.html
                //
                if (ofile != NULL) {
                    int out = open(ofile, O_WRONLY | O_CREAT | O_TRUNC, 0644); //this is just to make sure that the output file is created even when it does not exists.
                    if (out < 0) { //owronly open the file for write only access, ocreate creates a file if not exist and otrunc check if exist clear the file and put new content
                        perror("Failed to open the output file");//644 is permission owner group other
                        exit(1);
                    }
                    dup2(out, STDOUT_FILENO);
                    close(out);
                }

                execvp(prog_argv[0], prog_argv);
                perror("exec failed");
                exit(1);

            } 
            else if (pid){
                if (bg){
                     jobs[jobcounter].id = jobid++;
                jobs[jobcounter].pid = pid;
                strcpy(jobs[jobcounter].status, "Running");
                strcpy(jobs[jobcounter].command, prev);
                printf("[%d] %d\n", jobs[jobcounter].id, pid);
                jobcounter++;
                }
                pid_fg=pid;
                /* 
                * We're in the parent; let's wait for the child to finish*/
               //thanks to this website https://www.geeksforgeeks.org/exit-status-child-process-linux/ //reference for zombie,wifexited etc
               //note 1 : Miscellaneous errors, such as "divide by zero" and other impermissible operations.
                // 2 : Missing keyword or command, or permission problem.
                // 126 : Permission problem or command is not an executable
                // 128 : invalid argument to exit.
                int status;
                waitpid(pid, &status, WUNTRACED);
                pid_fg=0;//resett when done
                if(WIFEXITED(status)){
                    // 
                    prev_status=WEXITSTATUS(status);
                }else if (WIFSIGNALED(status)){
                    prev_status=128+WTERMSIG(status);
                }else if (WIFSTOPPED(status)){
                    int idx = find_job_with_pid(pid);
                    if (idx >= 0) {
                        strcpy(jobs[idx].status, "Stopped");
                        printf("\n[%d]+  Stopped\t\t%s\n", jobs[idx].id, jobs[idx].command);
                        fflush(stdout);
                    }
                    prev_status = 128 + WSTOPSIG(status);
                    
                }
                else{
                    prev_status=1;
                }
            }
        }
    }

    if (check_script) {
        fclose(input);
    }
    
    return 0;
}
