/* ICCS227: Project 1: icsh
 * Name: Teeratass Peysantiwong
 * StudentID: 6480651
 */

#include "stdio.h"
#include <string.h>
#define MAX_CMD_BUFFER 255

int main() {
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
                printf("%s ", command[i]);
            }
            printf("\n");
        }
        else{
            printf("bad command\n");
        }
    }
}
