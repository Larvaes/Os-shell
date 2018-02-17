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
void signal_handle(){
    printf("\nCtrl + c detect : Terminate program\n");
    exit(0);
}

void interactiveMode(char *command_all){
    char *token;
    char command[20];
    char sub_command[10][20];
    char *arg[10][10];  
    int index_arg = 0;
    int num_arg = 0;
    int status;
    strcpy(command,path); //copy path("/bin/..(command)..") to command for execute 
    if(strstr(command_all,";") != NULL){ //check if mutiple command exist (call from batch)
        token = strtok(command_all, ";"); //substring for each command in line
        while(token != NULL){
            strcpy(sub_command[num_arg],token);
            removeLineFeed(sub_command[num_arg++]);
            token = strtok(NULL, ";");       
        }
    }
    else{
        strcpy(sub_command[0],command_all);
        num_arg++;
    }
    //loop execute command in line (1 if call from interactive, multiple if call from batch and ";" exist)
    for(int i = 0 ; i < num_arg; i++){ 
        token = strtok(sub_command[i], " "); //substring for each argument
        while(token != NULL){
            arg[i][index_arg] = token; 
            token = strtok(NULL, " "); 
            index_arg++;       
        }
        arg[i][index_arg] = NULL;  //insert NULL into argument array
        strcat(command,arg[i][0]); //concatenate command to path for execute
        int pid = fork(); //fork new process and use child process for execute command
        if(pid == 0){               
            execvp(command,arg[i]);
            perror("Error! command not found ");    //print error to stderr if occured   
            exit(1);
        }
        wait(&status);
        strcpy(command,path);  //clear command string back to "/bin/" 
        index_arg = 0;
    }   

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
        signal(SIGINT,signal_handle);
        fgets(buff,BUFLEN,stdin); //get input command from user
        removeLineFeed(buff);
        if(strstr(buff, "quit") != NULL) //check for terminate program
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