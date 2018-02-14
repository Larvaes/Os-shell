#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

#define BUFLEN 1024

char buff[BUFLEN];
char *path = "/bin/";

void removeLineFeed(char *buff){
    size_t length = strlen(buff);
    if(buff[length - 1] == '\n')
        buff[length - 1] = '\0';
}

void interactiveMode(char *token, char *buff){
    char command[20];
    char *arg[10][50];  
    int index_arg = 0;
    int num_arg = 0;
    int status;
    strcpy(command,path);

    while(token != NULL){
            arg[num_arg][index_arg] = token;
            if(!strcmp(token,";")){
                arg[num_arg][index_arg] = NULL;
                num_arg++;
                index_arg = 0;
                token = strtok(NULL, " "); 
            }
            else{
                token = strtok(NULL, " ");
                index_arg++;
            }
        }
    arg[num_arg][index_arg] = NULL;
    for(int i = 0; i <= num_arg; i++){
        strcat(command,arg[i][0]);
        int pid = fork();
        if(pid == 0){
            execvp(command,arg[i]);
            perror("Error! ");
            exit(1);
        }
        wait(&status);
        strcpy(command,path);
    }
}

void batchMode(char *token){
    FILE *fp = fopen(token,"r");
    if(fp == NULL){
        perror("Open file ERROR ");
        return;
    }
    while(fgets(buff,BUFLEN,fp) != NULL){
        
        removeLineFeed(buff);
        token = strtok(buff, " ");
        interactiveMode(token,buff);

    }
}


int main(){

    char *token;
    while(1){   
        printf("prompt> ");
        fgets(buff,BUFLEN,stdin);
        removeLineFeed(buff);
        if(!strcmp(buff, "quit"))
            break;
        token = strtok(buff, " ");
        if(strstr(token,"./") != NULL)
            batchMode(token);
        else
            interactiveMode(token,buff);
    


    }
}