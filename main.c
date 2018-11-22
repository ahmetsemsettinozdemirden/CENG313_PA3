#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

// constants
const int BUILTIN_COMMANDS_SIZE = 5;
const char* BUILTIN_COMMANDS[BUILTIN_COMMANDS_SIZE] = { "cd", "dir", "history", "findloc", "bye" };

// global variables
char input[100];
char* tokenizedInput[10];

// functions
char** parseInput(char input[100]);
int isBuiltIn(char* parsedInput[]);
void execute();
void cd();
void dir();
void history();
void findloc();
void bye();

int main() {

    while(1) {
        setbuf(stdout, 0);
        printf("bestshellever>");

        // get line
        fgets(input, 100, stdin);
        input[strcspn(input, "\n")] = 0;

        // tokenize
        char copyInput[100];
        strcpy(copyInput, input);
        int i = 0;
        char* token = strtok(input, " ");
        while(token != NULL && i < 10) {
            tokenizedInput[i] = token;
            token = strtok(NULL, " ");
            i++;
        }

        // false is represented by 0
        // TODO: builtin as a background task
        if(isBuiltIn(tokenizedInput) != 0) { // TODO: how to do pipe `|` operation
            execute(tokenizedInput);
        } else {
//            fork();
//            if(child) {
//
//            } else if (not a background process) {
//                wait for the child
//            }
        }

        exit(0);
    }

}

int isBuiltIn(char* parsedInput[]) {
    // TODO: pipe `|` operation
    char* command = parsedInput[0];
    for (int i = 0; i < BUILTIN_COMMANDS_SIZE; ++i)
        if (strcmp(command, BUILTIN_COMMANDS[i]) == 0)
            return 1;
    return 0;
}

void execute(char* parsedInput[]) {
    // TODO: pipe `|` operation
    char* command = parsedInput[0];
    if (strcmp(command, "cd") == 0) {
        cd();
    } else if (strcmp(command, "dir") == 0){
        dir();
    } else if (strcmp(command, "history") == 0){
        history();
    } else if (strcmp(command, "findloc") == 0){
        findloc();
    } else if (strcmp(command, "bye") == 0){
        bye();
    } else {
        perror("command is not builtin.");
    }
}

void cd() {

    // usage: cd <directory>
    char* directory = tokenizedInput[1];

    // get home directory if <directory> does not exist
    if(directory == NULL || strcmp(directory, "") == 0) {
        directory = getenv("HOME");
    }

    // change directory
    chdir(directory);

    // get current absolute path
    char cwd[100];
    getcwd(cwd, sizeof(cwd));

    // update PWD
    setenv("PWD", cwd, 1);
}

void dir() {
    char cwd[100];
    if(getcwd(cwd, sizeof(cwd))) {
        setbuf(stdout, 0);
        printf(cwd);
    } else {
        setbuf(stdout, 0);
        perror("'dir' command couldn't executed.");
    }
}

void history() {
    // TODO: history
    setbuf(stdout, 0);
    printf("'history' command executed.");
}

void findloc() {
    // TODO: findloc
    setbuf(stdout, 0);
    printf("'findloc' command executed.");
}

void bye() {
    setbuf(stdout, 0);
    printf("bye");
    exit(0);
}