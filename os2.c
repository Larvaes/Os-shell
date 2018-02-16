#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

#define BUFLEN 1024

char buff[BUFLEN];
char *path = "/bin/";

void removeLineFeed(char *buff){     //for cut Linefeed("\n") at the at of string)
    size_t length = strlen(buff);
    if(buff[length - 1] == '\n')
        buff[length - 1] = '\0';
}

void interactiveMode(char *command_all){
    char *token;
    char command[20];
    char *arg[10];  
    int index_arg = 0;
    int status;
    strcpy(command,path); //copy path("/bin/..(command)..") to command for execute 

    token = strtok(command_all, " "); //substring for each argument
    while(token != NULL){
        arg[index_arg] = token; 
        token = strtok(NULL, " "); 
        index_arg++;       
    }
    arg[index_arg] = NULL;  //insert NULL into argument array
    strcat(command,arg[0]); //concatenate command to path for execute
    int pid = fork(); //fork new process and use child process for execute command
    if(pid == 0){               
        execvp(command,arg);
        perror("Error! ");    //print error to stderr if occured   
        exit(1);
    }
    wait(&status);
    strcpy(command,path);  //clear command string back to "/bin/"      

}

void batchMode(char *file){        
    FILE *fp = fopen(file,"r");    //open file from first argument 
    if(fp == NULL){
        perror("Open file ERROR "); //check if file cannot open 
        return;
    }
    while(fgets(buff,BUFLEN,fp) != NULL){   //read command in each line of file
        removeLineFeed(buff);
        interactiveMode(buff);    //call interactive mode for each command line
    
            
    }
}


int main(){

    char *token;
   
    while(1){
        char command_all[10][20];
        int cm_index = 0;   
        printf("prompt> ");
        fgets(buff,BUFLEN,stdin); //get input command from user
        removeLineFeed(buff);
        if(!strcmp(buff, "quit")) //check for terminate program
            break;
        token = strtok(buff, ";");
        while(token != NULL){  //loop substring for each command divide by ";"
            strcpy(command_all[cm_index],token);
            removeLineFeed(command_all[cm_index++]);
            token = strtok(NULL, ";");
        }
        for(int i = 0; i < cm_index; i++){  //loop execute each command
            if(strstr(command_all[i],"./") != NULL){ //check if command is "./" for switch to batch mode
                token = strtok(command_all[i]," "); //delete whitespace before "./" if exist
                batchMode(token);     
            }     
            else
                interactiveMode(command_all[i]); 
        }
        


    }
}