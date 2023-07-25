#ifndef SOURCE_H
#define SOURCE_H

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>
#include <stdlib.h>
#include <ctype.h>
#include <signal.h>
#include <fcntl.h> 
#include <dirent.h>
#include <libgen.h>

// Define a struct to hold information about a parsed command
typedef struct {
    int commandsNum;     // the number of commands
    int tokens1Num;      // the number of tokens in tokens1
    char **tokens1;      // an array of tokens1 -strings
    int tokens2Num;      // the number of tokens in tokens2
    char **tokens2;      // an array of tokens2 -strings
} parseInfo;

// Declare global variables and functions
extern int signalChildPid;
void sigintHandler(int sig_num);
char* readline(char* input);
parseInfo* parse(char *cmdLine);
void executeCommand(parseInfo* info);
void cd(parseInfo* info);
void cat(parseInfo* info);
void ls(parseInfo* info);
void tokenize(char ***tokensPtr, int *tokensNumPtr, char *string);
void execute_pipe(parseInfo* info);
void freeInfo(parseInfo *info);
void print_tree(char *path, int level);
#endif