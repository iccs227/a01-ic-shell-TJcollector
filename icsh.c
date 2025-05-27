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
        printf("you said: %s\n", buffer);
    }
}
