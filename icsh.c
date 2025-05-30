/* ICCS227: Project 1: icsh
 * Name: Teeratass Peysantiwong
 * StudentID: 6480651
 */

#include "stdio.h"
#include <string.h>
#include <stdlib.h>
#define MAX_CMD_BUFFER 255

int main(int argc, char * argv[]) {
    char prev[MAX_CMD_BUFFER]="";
    char buffer[MAX_CMD_BUFFER];
    while (1) {
        printf("icsh $ ");
        if(fgets(buffer, MAX_CMD_BUFFER,stdin)== NULL){
            printf("\n");
            break;
        }
        buffer[strcspn(buffer, "\n")]=0;
        if(strlen(buffer)==0){
            continue;
        }
        if (strcmp(buffer, "!!") == 0) {
            if (strlen(prev) == 0) {
                continue;  
            }
            printf("%s\n", prev);
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
    
        if(strcmp(command[0],"echo")==0){
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
            printf("\033[1;31mbye\033[0m\n");
            return code;
        }
        else{
            printf("\033[1;33mbad command\033[0m\n");
            // printf("bad command\n");
        }
    }
}
