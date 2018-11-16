#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

// constants
const char* SHELL_NAME = "bestshellever";
const int BUILTIN_COMMANDS_SIZE = 5;
const char* BUILTIN_COMMANDS[BUILTIN_COMMANDS_SIZE] = { "cd", "dir", "history", "findloc", "bye" };

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
        printf(strcat(SHELL_NAME, ">"));

        char input[100];
        scanf("%s", input); // TODO: check 100 chars limit

        char** parsedInput;
        parsedInput = parseInput(input); // TODO: check 10 words limit

        // TODO: builtin as a background task
        if(isBuiltIn(parsedInput) == 0) { // TODO: how to do pipe `|` operation
            execute(parsedInput);
        } else {
//            fork();
//            if(child) {
//
//            } else if (not a background process) {
//                wait for the child
//            }
        }

    }

}

char** parseInput(char input[100]) {

    int charCounter = 0;
    int wordCounter = 0;

    // TODO: parse input into parsedInput

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
    if (strcmp(command, "cd")) {
        cd();
    } else if (strcmp(command, "dir")){
        dir();
    } else if (strcmp(command, "history")){
        history();
    } else if (strcmp(command, "findloc")){
        findloc();
    } else if (strcmp(command, "bye")){
        bye();
    } else {
        perror("command is not builtin.");
    }
}

void cd() {
    // TODO: cd
    printf("'cd' command executed.");
}

void dir() {
    char cwd[100];
    if(getcwd(cwd, sizeof(cwd))) {
        printf(cwd);
    } else {
        perror("'dir' command couldn't executed.");
    }
}

void history() {
    // TODO: history
    printf("'history' command executed.");
}

void findloc() {
    // TODO: findloc
    printf("'findloc' command executed.");
}

void bye() {
    printf("bye");
    exit(0);
}