// source.c
#include <stdlib.h>
#include "source.h"
#include <stdio.h>

extern int signalChildPid;

//A function to execute second command
void executeCommand2(parseInfo* info){
    // Check if the command is "cat" and the output needs to be redirected
    if(strcmp(info->tokens2[0],"cat") == 0){
		cat(info);
		exit(EXIT_SUCCESS);
	}
    // Check if the command is "cd"
	else if(strcmp(info->tokens2[0],"cd") == 0){
        exit(EXIT_SUCCESS);
	}
    // Check if the command is "exit"
	else if(strcmp(info->tokens2[0],"exit") == 0){
		exit(EXIT_SUCCESS);
	}
    // Check if the command is "ls" and has additional arguments
    else if((strcmp(info->tokens2[0],"ls")==0) && info->tokens2Num >2){
        ls(info);
    }
    // Execute the command using the "execvp" system call
	else if(execvp(info->tokens2[0], info->tokens2) == -1 ){
		perror("error");
		exit(EXIT_FAILURE);
	}
    // Exit the process
	exit(EXIT_SUCCESS);
}

// Reads a line of input from the user
char* readline(char* input) {

    printf("Enter a command %s ",input); 

    char* cmd = (char*)malloc(sizeof(char) * 1); // Allocate memory for cmd
	int c , i=0;
	while((c = getchar()) != '\n' && c != EOF ){
        // Append the read character to cmd and increase i
    	cmd[i++] = c;
        // Reallocate memory for cmd to store the new character
        cmd = realloc(cmd, i+1);
		if(cmd == NULL){
			printf("No memory for allocation\n");
			exit(EXIT_FAILURE);
		}
    }
 
    cmd[i] = '\0';
    return cmd;
}

// Parses the command line and returns the information
parseInfo *parse(char *cmdLine) {
    parseInfo *info = malloc(sizeof(parseInfo));
    if (info == NULL) {
        printf("Error: Could not allocate memory for parseInfo\n");
        return NULL;
    }

    char *token, *tokens1 = NULL, *tokens2 = NULL, *tempCmdLine1 = strdup(cmdLine), *tempCmdLine2 = strdup(cmdLine);

    // Get the first token
    token = strtok(tempCmdLine1, "|");

    info->commandsNum = 0;

    // Keep getting tokens until there are no more
    while (token != NULL) {
        info->commandsNum++;
        token = strtok(NULL, "|");
    }

    // handle single command 
    if (info->commandsNum == 1) {
        tokenize(&info->tokens1, &info->tokens1Num, tempCmdLine2);

    // handle double commands separated by a pipe
    } else if (info->commandsNum == 2) {
        tokens1 = strtok(tempCmdLine2, "|");

        tokenize(&info->tokens1, &info->tokens1Num, tokens1);

        // tokenize the second command
        tokens2 = strtok(cmdLine, "|");
        tokens2 = strtok(NULL, "|");
        tokenize(&info->tokens2, &info->tokens2Num, tokens2);   
    }else{
        printf("Error: unsupported number of commands!\n");
        free(info);
        free(tempCmdLine1);
        free(tempCmdLine2);
        return NULL;
    } 

    // free temporary strings and return the parseInfo structure
    free(tempCmdLine1);
    free(tempCmdLine2);
    return info;
}

// Tokenizes a string into an array of strings (tokens)
void tokenize(char ***tokensPtr, int *tokensNumPtr, char *string){
    char *token, *tempCmdLine=strdup(string);
    // parse command name
    token = strtok(tempCmdLine, " ");

    // parse arguments
    *tokensNumPtr = 1;
    while ((token = strtok(NULL, " ")) != NULL) {
        (*tokensNumPtr)++;
    }
    
    // allocate memory for tokens
    *tokensPtr = malloc((*tokensNumPtr + 1) * sizeof(char *));
    
    // iterate through string and add tokens to array
    int i = 0;
    token = strtok(string," ");

    while (token != NULL) {
        (*tokensPtr)[i] = strdup(token); // allocate memory for token and copy it to the array   

        token = strtok(NULL," ");
        i++;
    }
    (*tokensPtr)[i] = NULL; // set last element to NULL to indicate end of array
    free(tempCmdLine); // free temporary string
}

// Executes the command based on the information provided in the parseInfo struct
void executeCommand(parseInfo* info){
    if(strcmp(info->tokens1[0],"cat") == 0){
        // If the command is "cat > file", redirect output to file
		cat(info);
		exit(EXIT_SUCCESS);
	}
	else if(strcmp(info->tokens1[0],"cd") == 0){ 
        // If the command is "cd", exit with success status
        exit(EXIT_SUCCESS);
	}
	else if(strcmp(info->tokens1[0],"exit") == 0){
        // If the command is "exit", exit with success status
		exit(EXIT_SUCCESS);
	}
    else if((strcmp(info->tokens1[0],"ls")==0) && info->tokens1Num >2){
        // If the command is "ls" and has more than 2 tokens, use custom implementation
        ls(info);
    }
    else if((strcmp(info->tokens1[0],"tree")==0)){
        // If the command is "tree", execute custom implementation

         if(info->tokens1Num != 1){ //Checking for incorrect number of arguments
            printf("Error: incorrect number of arguments for command: tree\n");
            exit(EXIT_FAILURE);
        }

        // Get current working directory path
        char path[1024];
        getcwd(path, 1024);
        ssize_t len = 0;  // Declare and initialize len to 0

        // Get the path of the "tree.c" file
        char cwd[1024];
        char *dir;
        len = readlink("/proc/self/exe", cwd, sizeof(cwd) - 1);
        if (len != -1) {
            cwd[len] = '\0';
            dir = dirname(cwd);
        } else {
            perror("readlink");
            exit(EXIT_FAILURE);
        }

        strcat(dir,"/tree.c");

        // Set up arguments to compile the "tree.c" file into an executable named "tree"
        char *gcc_args[] = {"gcc", "-o", "tree", dir, NULL};
        
        // Execute the "gcc -o tree tree.c" command
        pid_t gcc_pid = fork();
        if (gcc_pid == 0) {
            execvp(gcc_args[0], gcc_args);
        } else {
            waitpid(gcc_pid, NULL, 0);
        }

        // Set up arguments to run the "tree" executable
        char *tree_args[] = {"./tree", path, NULL};
        execvp(tree_args[0], tree_args);
    }
	else if(execvp(info->tokens1[0], info->tokens1) == -1 ){  
        // If the command is not one of the special cases above, attempt to execute it using execvp
		perror("error");
		exit(EXIT_FAILURE);
	}
    // Exit with success status after executing the command
	exit(EXIT_SUCCESS);
}

// Changes the current working directory
void cd(parseInfo* info){
	// Check if no arguments are provided, and change to the home directory
	if(info->tokens1Num == 1){
		chdir(getenv("HOME"));
		return;
	}
    // Check if too many arguments are provided
	else if(info->tokens1Num > 2){
		printf("cd: too many arguments\n");
		return;
	}
    // Change to the directory specified in the argument
	else if(chdir(info->tokens1[1]) == -1){
		printf("cd: %s: no such file or directory\n", info->tokens1[1]);
		return;
	}
}

// Lists the files and directories in the current directory
void ls(parseInfo* info){
        if (info->tokens1[2] != NULL && strcmp(info->tokens1[2], ">") == 0 && info->tokens1Num==4) {
            freopen(info->tokens1[3], "w", stdout);
            info->tokens1[2] = NULL; // remove "> text.txt" from argument list
        }
        else if(info->tokens1[1] != NULL && strcmp(info->tokens1[1], ">") == 0 && info->tokens1Num==3)  {
            freopen(info->tokens1[2], "w", stdout);
            info->tokens1[1] = NULL; // remove "> text.txt" from argument list
        }else {
            printf("ls: too many arguments\n");
			return;
        }
        execvp(info->tokens1[0], info->tokens1);
        perror("execvp failed");
        exit(EXIT_FAILURE);
}

// Prints the contents of a file to stdout
void cat(parseInfo* info){

    if(info->tokens1Num > 3 || info->tokens1Num ==1){
        printf("Error: incorrect number of arguments for command: cat \n");
        freeInfo(info);
        exit(EXIT_FAILURE);
    }
    else if(info->tokens1Num == 2){

        if (fopen(info->tokens1[1], "r")) { // Check if a file exists
                //if file exsits ,print content
            	if(execvp(info->tokens1[0], info->tokens1) == -1 ){  
                    perror("error");
                    exit(EXIT_FAILURE);
	            }
        }else{
            // Open the file specified in the command
            int fd = open(info->tokens1[1], O_RDWR | O_APPEND | O_CREAT | O_TRUNC, 0777);
       
            exit(EXIT_FAILURE);
        }
    }
    else if((info->tokens1Num == 3) && (strcmp(info->tokens1[1], ">")==0)){
        // Register signal handler for SIGINT (Ctrl+C)
        signal(SIGINT, &sigintHandler);

        // Open the file specified in the command
        int fd = open(info->tokens1[2], O_RDWR | O_APPEND | O_CREAT | O_TRUNC, 0777);

        if(fd < 0){      
            exit(EXIT_FAILURE);
        }

        // Loop indefinitely to read input character by character and write it to the file
        while(1){
            char c = getchar();
            // Exit if EOF is encountered (Ctrl+D)
            if(c == EOF){
                exit(EXIT_SUCCESS);
            }
            write(fd, &c, 1); // Write the character to the file
        }
          

    }
}

// Signal handler function for Ctrl+C signal
void sigintHandler(int sig_num){
   	if(signalChildPid == 0){
		printf("\n");
		exit(EXIT_SUCCESS);
	}
	printf("\n");
}

// Executes two commands connected by a pipe (|) operator
void execute_pipe(parseInfo* info) {
    int fd[2];

    if (pipe(fd) == -1) { // creates pipe and checks if creation fails
        perror("pipe failed"); // print error message
        exit(EXIT_FAILURE);
    }

    if (fork() == 0) {      // Child process 1
        close(STDOUT_FILENO);  //closing stdout
        dup(fd[1]);         //replacing stdout with pipe write 
        close(fd[0]);       //closing pipe read
        close(fd[1]);		//closing pipe write
        executeCommand(info); // execute first command
        exit(EXIT_SUCCESS);
    }

    if (fork() == 0) {      // Child process 2 
        close(STDIN_FILENO);   //closing stdin
        dup(fd[0]);         //replacing stdin with pipe read
        close(fd[1]);       //closing pipe write
        close(fd[0]);		//closing pipe read
        executeCommand2(info); // execute second command
        exit(EXIT_SUCCESS);
    }
    close(fd[0]);
    close(fd[1]);
    wait(0);
    wait(0);
}

// Frees the memory allocated for parseInfo struct
void freeInfo(parseInfo *info){
    for (int i = 0; i < info->tokens1Num; i++) {
    free(info->tokens1[i]); // free memory allocated for tokens1
    }
    free(info->tokens1); // free memory allocated for array of tokens1
    for (int i = 0; i < info->tokens2Num; i++) {
        free(info->tokens2[i]); // free memory allocated for tokens2
    }
    free(info->tokens2); // free memory allocated for array of tokens2
    free(info); // free memory allocated for the parseInfo struct
}

