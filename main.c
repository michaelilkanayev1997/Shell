// main.c
#include "source.h"

int signalChildPid=1;
int ctrl_c=0;

int main(int argc, char **argv) {
  
    int childPid,status; 
    char * cmdLine;
    parseInfo *info; 

    signal(SIGINT, &sigintHandler); //handle SIGINT signal (CTRL+C)
	while(1){ 
        fflush(stdout); // flush stdout before scanning for input
        cmdLine= readline(">");  // read command line input from user

        if (strlen(cmdLine) == 0 ) { // if command line input is empty, skip iteration and prompt user for another command
            continue;
        }

        info = parse(cmdLine); // parse command line input into tokens and store in parseInfo struct

        if(!info){ // if parsing failed, skip iteration and prompt user for another command
           continue;
        }
    
        if(info->commandsNum == 1){ // if there is only 1 command
            if(strcmp(info->tokens1[0],"cd") == 0){ // if command is 'cd', change directory using cd() function
                cd (info);
            }
            else if(strcmp(info->tokens1[0],"exit") == 0){ // if command is 'exit', exit the program
                 if(info->tokens1Num != 1){
                    printf("Error: incorrect number of arguments for command: exit\n");
                    continue;
                }
                exit(EXIT_SUCCESS);

            }else{ // if command is not 'cd' or 'exit', execute the command using fork() and execvp()
                childPid = fork();   

                signalChildPid = childPid; // store child process id in global variable signalChildPid for use in sigintHandler()
                if (childPid == 0){
                    /* child code */
                    executeCommand(info); // execute command 
                    exit(EXIT_SUCCESS);
                }else { 
                    /* parent code */
                    if ( waitpid(childPid, &status, WUNTRACED | WCONTINUED ) == -1) { // wait for child process to finish execution
                        perror("waitpid");
                        exit(EXIT_FAILURE);
                    }
                }	
            }     
        }else{ // if there are 2 commands separated by pipe '|', execute both commands using pipes
            execute_pipe(info); // execute commands using pipes
        }
    }
    freeInfo(info); // free memory allocated for parseInfo struct
    return 0;
}