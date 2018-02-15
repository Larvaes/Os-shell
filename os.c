#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

#define BUFLEN 1024

char buff[BUFLEN];
char *path = "/bin/";

void removeLineFeed(char *buff){     //for cut Linefeed("\n") at the at of string)
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
    strcpy(command,path);      //copy path("/bin/..(command)..") to command for execute 

    while(token != NULL){
            arg[num_arg][index_arg] = token; //copy substring in to argument array
            if(!strcmp(token,";")){          //check if found ";" for begin ne command
                arg[num_arg][index_arg] = NULL; //insert NULL into last command's argument
                num_arg++;          //increase command index
                index_arg = 0;      //set argument index to 0 for new command
                token = strtok(NULL, " ");  //substring next argument
            }
            else{
                token = strtok(NULL, " "); //substring next argument
                index_arg++;       //increase argument index
            }
        }
    arg[num_arg][index_arg] = NULL;
    for(int i = 0; i <= num_arg; i++){
        strcat(command,arg[i][0]);  //concatenate command to path for execute
        int pid = fork();
        if(pid == 0){               //fork new process and use child process for execute command
            execvp(command,arg[i]);
            perror("Error! ");      //print error to stderr if occured
            exit(1);
        }
        wait(&status);
        strcpy(command,path);       //clear command string back to "/bin/"
    }
}

void batchMode(char *token){        
    FILE *fp = fopen(token,"r");    //open file from first argument 
    if(fp == NULL){
        perror("Open file ERROR "); //check if file cannot open 
        return;
    }
    while(fgets(buff,BUFLEN,fp) != NULL){   //read command in each line of file
        
        removeLineFeed(buff);
        token = strtok(buff, " ");
        interactiveMode(token,buff);    //call interactive mode for each command line

    }
}


int main(){

    char *token;
    while(1){   
        printf("prompt> ");
        fgets(buff,BUFLEN,stdin);   //get string command either batch or interactive mode 
        removeLineFeed(buff);       
        if(!strcmp(buff, "quit"))   //check for terminate program
            break;
        token = strtok(buff, " ");  //substring input into command and argument
        if(strstr(token,"./") != NULL) //check if command is "./" for switch to batch mode
            batchMode(token);          //call batch mode
        else
            interactiveMode(token,buff); //call interactivemode
    


    }
}